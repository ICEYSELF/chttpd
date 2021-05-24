/**
 * Basic grammar of configuration language:
 *   configuration ::= lines
 *   lines ::= lines line | NIL
 *   line ::= router-line | filter-line | config-line
 *   router-line ::= method PATH handler-type HANDLER
 *   filter-line ::= "filter" PATH filter-type FILTER maybe-exact
 *   method ::= "get" | "post"
 *   handler-type ::= "script" | "dcgi" | "static"
 *   filter-type ::= "script" | "dcgi"
 *   maybe-exact ::= "exact" | NIL
 *   config-line ::= "listen-address" ADDRESS
 *                 | "listen-port" PORT
 *                 | "max-pending" MAX-PENDING
 */

#ifndef CHTTPD_CONFIG_H
#define CHTTPD_CONFIG_H

#include "cc_vec.h"
#include "http_base.h"
#include "pl2b.h"
#include "util.h"

#define CHTTPD_VER_MAJOR 0
#define CHTTPD_VER_MINOR 1
#define CHTTPD_VER_PATCH 0

#define CHTTPD_NAME        "chttpd"
#define CHTTPD_SERVER_NAME "chttpd/0.1"

typedef enum e_handler_type {
  HDLR_SCRIPT = 0,
  HDLR_STATIC = 1,
  HDLR_DCGI   = 2
} HandlerType;

extern const char *HANDLER_TYPE_NAMES[];

typedef struct st_route {
  HttpMethod httpMethod;
  const char *path;
  HandlerType handlerType;
  const char *handlerPath;
} Route;

typedef struct st_config {
  const char *address;
  int port;
  int maxPending;

  ccVec TP(Route) routes;
} Config;

void initConfig(Config *config);
void dropConfig(Config *config);

const pl2b_Language *getCfgLanguage(void);

#endif /* CHTTPD_CONFIG_H */
