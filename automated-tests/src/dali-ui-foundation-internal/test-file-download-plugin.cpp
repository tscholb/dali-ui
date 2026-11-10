/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <dali/devel-api/adaptor-framework/file-download-plugin.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <fstream>
#include <mutex>

namespace
{
using CompletionCallback = void (*)(int, bool, const char*, void*);
std::mutex              gMutex;
std::condition_variable gCondition;
CompletionCallback      gCallback{nullptr};
void*                   gUserData{nullptr};
int                     gDownloadId{0};
std::atomic_uint        gSyncDownloads{0u};

class TestFileDownloadPlugin : public Dali::FileDownloadPlugin
{
public:
  bool InitializePlugin() override
  {
    return true;
  }

  bool DownloadRemoteFileIntoMemory(const std::string&, Dali::Vector<uint8_t>& buffer,
                                    size_t& size, size_t maximumSize) override
  {
    ++gSyncDownloads;
    const char* path = std::getenv("DALI_TEST_REMOTE_FILE_DOWNLOAD_PATH");
    if(!path)
    {
      return false;
    }
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    const auto    length = file.tellg();
    if(length <= 0 || static_cast<size_t>(length) > maximumSize)
    {
      return false;
    }
    size = static_cast<size_t>(length);
    buffer.Resize(size);
    file.seekg(0);
    return static_cast<bool>(file.read(reinterpret_cast<char*>(buffer.Begin()), size));
  }
};
} // namespace

extern "C" Dali::FileDownloadPlugin* CreateFileDownloadPlugin()
{
  return new TestFileDownloadPlugin;
}

extern "C" bool InitializeFileDownloadPlugin(Dali::FileDownloadPlugin* plugin)
{
  return plugin->InitializePlugin();
}

extern "C" void DestroyFileDownloadPlugin(Dali::FileDownloadPlugin* plugin)
{
  delete plugin;
}

extern "C" bool FileDownloadPluginIsAsyncDownloadSupported()
{
  return !std::getenv("DALI_TEST_REMOTE_ASYNC_UNSUPPORTED");
}

extern "C" int FileDownloadPluginStartAsyncDownload(Dali::FileDownloadPlugin*, const char*, size_t,
                                                    CompletionCallback callback, void* userData)
{
  if(std::getenv("DALI_TEST_REMOTE_START_FAIL"))
  {
    return -1;
  }
  std::lock_guard<std::mutex> lock(gMutex);
  gCallback = callback;
  gUserData = userData;
  ++gDownloadId;
  gCondition.notify_all();
  return gDownloadId;
}

extern "C" void FileDownloadPluginCancelAsyncDownload(Dali::FileDownloadPlugin*, int)
{
  std::lock_guard<std::mutex> lock(gMutex);
  gCallback = nullptr;
  gUserData = nullptr;
}

extern "C" bool TestFileDownloadWaitForRequest()
{
  std::unique_lock<std::mutex> lock(gMutex);
  return gCondition.wait_for(lock, std::chrono::seconds(5), []
  { return gCallback != nullptr; });
}

extern "C" void TestFileDownloadComplete(bool success, const char* path)
{
  CompletionCallback callback;
  void*              userData;
  int                downloadId;
  {
    std::lock_guard<std::mutex> lock(gMutex);
    callback   = gCallback;
    userData   = gUserData;
    downloadId = gDownloadId;
    gCallback  = nullptr;
    gUserData  = nullptr;
  }
  if(callback)
  {
    callback(downloadId, success, path, userData);
  }
}

extern "C" unsigned int TestFileDownloadSyncCount()
{
  return gSyncDownloads.load();
}
