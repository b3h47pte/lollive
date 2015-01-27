#pragma once
#ifndef _URLFETCH_H
#define _URLFETCH_H

#include <iostream>
#include <curl/curl.h>

// TODO: Maybe develop this into a class someday.
struct MemoryStruct {
  char* memory;
  size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;
    mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

static MemoryStruct* DownloadData(const std::string& url) {
  MemoryStruct* mem = new MemoryStruct();
  CURL* curl = curl_easy_init();
  CURLcode res;
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)mem);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
  }
  return mem;
}

// TODO: Test this function.
static void DownloadTextFile(const std::string& url, std::string& outString) {
  MemoryStruct* mem = DownloadData(url);
  outString = std::string(mem->memory);
  delete mem->memory;
  delete mem;
}

#endif
