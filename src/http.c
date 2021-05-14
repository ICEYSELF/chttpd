#include "http.h"
#include "util.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const char *HTTP_METHOD_NAMES[] = {
  [HTTP_GET] = "GET",
  [HTTP_POST] = "POST"
};

const char *HTTP_CODE_NAMES[] = {
  [HTTP_CODE_OK] = "Ok",
  [HTTP_CODE_NO_CONTENT] = "No Content",
  [HTTP_CODE_BAD_REQUEST] = "Bad Request",
  [HTTP_CODE_UNAUTHORIZED] = "Unauthorised",
  [HTTP_CODE_FORBIDDEN] = "Forbidden",
  [HTTP_CODE_NOT_FOUND] = "Not Found",
  [HTTP_CODE_SERVER_ERR] = "Internal Server Error"
};

static _Bool parseHttpFirstLine(const char *line,
                                HttpMethod *method,
                                ccVec TP(StringPair) *params);
static const char *skipWhitespace(const char *start);
static _Bool parseQueryPath(const char *start,
                            const char *end,
                            ccVec TP(StringPair) *params);

HttpRequest *readHttpRequest(FILE *fp) {
  char *firstLine = NULL;
  ssize_t lineSize = getdelim(&firstLine, 0, '\n', fp);

  if (lineSize == -1) {
    if (errno != 0) {
      LOG_ERR("error: getdelim: %d", errno);
      free(firstLine);
      return NULL;
    }
  }

  HttpMethod method;
  ccVec TP(StringPair) params;
  ccVec TP(StringPair) headers;

  ccVecInit(&params, sizeof(StringPair));
  ccVecInit(&headers, sizeof(StringPair));

  // TODO
  return NULL;
}

// GET /index.html?a=param1&b=param2 HTTP/1.1 \r\n\r\n
static _Bool parseHttpFirstLine(const char *line,
                                HttpMethod *method,
                                ccVec TP(StringPair) *params) {
  size_t lineLength = strlen(line);

  const char *it = strchr(line, ' ');
  if (it == NULL) {
    LOG_ERR("error parsing http request: \"%s\": missing first space",
            line);
    return 0;
  }

  if (strncmp(line, "GET", it - line)) {
    *method = HTTP_GET;
  } else if (strncmp(line, "POST", it - line)) {
    *method = HTTP_POST;
  } else {
    LOG_ERR("error parsing http request: \"%s\": unsupported method",
            line);
    return 0;
  }

  it = skipWhitespace(it);
  const char *it2 = strchr(it, '/');
  if (it2 == NULL) {
    LOG_ERR("error parsing http request: \"%s\": missing path",
            line);
    return 0;
  }
  it = it2;
  it2 = strchr(it2, ' ');

  if (!parseQueryPath(it, it2, params)) {
    LOG_ERR("error parsing http request: \"%s\": invalid query path",
            line);
    return 0;
  }

  return 1;
}

static const char *skipWhitespace(const char *str) {
  while (isspace(*str)) {
    ++str;
  }
  return str;
}

static _Bool parseQueryPath(const char *it1,
                            const char *it2,
                            ccVec TP(StringPair) *params) {
  const char *it3 = it1;
  while (it3 != it2 && *it3 != '?') {
    it3++;
  }
  
  size_t pathSize = it3 - it1;
  char *path = (char*)malloc(pathSize + 1);
  strncpy(path, it1, pathSize);

  StringPair *pair = (StringPair*)malloc(sizeof(StringPair));
  pair->first = copyString("!!reserved0");
  pair->second = path;

  ccVecPushBack(params, pair);

  if (*it3 != '?') {
    return 1;
  }

  it3++;
  for (;;) {
    it1 = it3;
    while (it3 != it2 && *it3 != '=') {
      it3++;
    }

    if (*it3 != '=') {
      LOG_ERR("error parsing query parameter: \"=\" expected");
      return 0;
    }
    const char *it4 = it3 + 1;
    while (it4 != it2 && *it4 != '&') {
      it4++;
    }
 
    size_t keySize = it3 - it1;
    size_t valueSize = it4 - it3 - 1;
    char *key = (char*)malloc(keySize + 1);
    char *value = (char*)malloc(valueSize + 1);
    strncpy(key, it1, keySize);
    strncpy(key, it3 + 1, valueSize);
    
    StringPair *param = (StringPair*)malloc(sizeof(StringPair));
    param->first = key;
    param->second = value;
    ccVecPushBack(params, param);

    if (*it4 != '&') {
      return 1;
    }
  
    it3 = it4 + 1;
  }
}

