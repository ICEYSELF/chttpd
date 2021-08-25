#include "intern.h"
#include "config.h"

#include <string.h>

extern const char *ERROR_PAGE_403_CONTENT =
"<html>\n"
"  <meta charset=\"utf-8\">\n"
"  <body>\n"
"    <div align=\"center\">\n"
"      <h2>403 Forbidden</h2>\n"
"      <hr/>\n"
"      Powered by chttpd: https://github.com/ICEYSELF/chttpd\n"
"    </div>\n"
"  </body>\n"
"</html>";

extern const char *ERROR_PAGE_404_CONTENT =
"<html>\n"
"  <meta charset=\"utf-8\">\n"
"  <body>\n"
"    <div align=\"center\">\n"
"      <h2>404 Not Found</h2>\n"
"      <hr/>\n"
"      Powered by chttpd: https://github.com/ICEYSELF/chttpd\n"
"    </div>\n"
"  </body>\n"
"</html>";

extern const char *ERROR_PAGE_500_CONTENT_PART1 =
"<html>\n"
"  <meta charset=\"utf-8\">\n"
"  <body>\n"
"    <div align=\"center\">\n"
"      <h2>500 Internal Server Error</h2>\n"
"      <hr/>\n"
"      <code style=\"text-align: left\"><pre>\n";

extern const char *ERROR_PAGE_500_CONTENT_PART2 =
"      </pre></code>\n"
"      Powered by chttpd: https://github.com/ICEYSELF/chttpd\n"
"    </div>\n"
"  </body>\n"
"</html>";

extern const char *GENERAL_HEADERS =
"Content-Type: text/html\r\n"
"Content-Encoding: identity\r\n"
"Cache-Control: public, max-age=1800\r\n"
"Connection: close\r\n\r\n";

extern const char *ERROR_PAGE_403_HEAD =
"HTTP/1.1 403 Forbidden\r\n";

extern const char *ERROR_PAGE_404_HEAD =
"HTTP/1.1 404 Not Found\r\n";

extern const char *ERROR_PAGE_500_HEAD =
"HTTP/1.1 500 Internal Server Error\r\n";

void send403Page(FILE *fp) {
  fputs(ERROR_PAGE_403_HEAD, fp);
  fprintf(fp, "Content-Length: %zu\r\n",
          strlen(ERROR_PAGE_403_CONTENT));
  fprintf(fp, "Server: %s\r\n", CHTTPD_SERVER_NAME);
  fputs(GENERAL_HEADERS, fp);
  fputs(ERROR_PAGE_403_CONTENT, fp);
}

void send404Page(FILE *fp) {
  fputs(ERROR_PAGE_404_HEAD, fp);
  fprintf(fp, "Content-Length: %zu\r\n",
          strlen(ERROR_PAGE_404_CONTENT));
  fprintf(fp, "Server: %s\r\n", CHTTPD_SERVER_NAME);
  fputs(GENERAL_HEADERS, fp);
  fputs(ERROR_PAGE_404_CONTENT, fp);
}

void send500Page(FILE *fp, Error *error) {
  fputs(ERROR_PAGE_500_HEAD, fp);
  fprintf(fp, "Server: %s\r\n", CHTTPD_SERVER_NAME);
  fputs(GENERAL_HEADERS, fp);
  fputs(ERROR_PAGE_500_CONTENT_PART1, fp);
  fprintf(fp, "%s:%zi: %s",
          error->sourceInfo.sourceFile,
          error->sourceInfo.line,
          error->errorBuffer);
  fputs(ERROR_PAGE_500_CONTENT_PART2, fp);
}

void handleIntern(const char *handlerPath, Error *error) {
  if (!strcmp(handlerPath, "403")) {
    QUICK_ERROR(error, 403, "user appointed");
  } else if (!strcmp(handlerPath, "404")) {
    QUICK_ERROR(error, 404, "user appointed");
  } else if (!strcmp(handlerPath, "500")) {
    QUICK_ERROR(error, 500, "user appointed");
  } else {
    QUICK_ERROR2(error, 500, "unsupported intern page: %s",
                 handlerPath);
  }
}

