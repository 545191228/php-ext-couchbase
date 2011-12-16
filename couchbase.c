/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright 2011 Couchbase, Inc.                                       |
  +----------------------------------------------------------------------+
  | Licensed under the Apache License, Version 2.0 (the "License");      |
  | you may not use this file except in compliance with the License.     |
  | You may obtain a copy of the License at                              |
  |     http://www.apache.org/licenses/LICENSE-2.0                       |
  | Unless required by applicable law or agreed to in writing, software  |
  | distributed under the License is distributed on an "AS IS" BASIS,    |
  | WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or      |
  | implied. See the License for the specific language governing         |
  | permissions and limitations under the License.                       |
  +----------------------------------------------------------------------+
  | Author: Xinchen Hui    <laruence@php.net>                            |
  +----------------------------------------------------------------------+
*/

/* $Id */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "libcouchbase/couchbase.h"
#include "php_couchbase.h"

ZEND_DECLARE_MODULE_GLOBALS(couchbase)

static int le_couchbase;

/** {{{ COUCHBASE_ARG_INFO
 */
COUCHBASE_ARG_PREFIX
ZEND_BEGIN_ARG_INFO_EX(arginfo_connect, 0, 0, 1)
    ZEND_ARG_INFO(0, host)
    ZEND_ARG_INFO(0, user)
    ZEND_ARG_INFO(0, password)
    ZEND_ARG_INFO(0, bucket)
ZEND_END_ARG_INFO()

COUCHBASE_ARG_PREFIX
ZEND_BEGIN_ARG_INFO_EX(arginfo_add, 0, 0, 3)
    ZEND_ARG_INFO(0, resource)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
    ZEND_ARG_INFO(0, expiration)
ZEND_END_ARG_INFO()

COUCHBASE_ARG_PREFIX
ZEND_BEGIN_ARG_INFO_EX(arginfo_set, 0, 0, 3)
    ZEND_ARG_INFO(0, resource)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
    ZEND_ARG_INFO(0, expiration)
    ZEND_ARG_INFO(0, cas)
ZEND_END_ARG_INFO()

COUCHBASE_ARG_PREFIX
ZEND_BEGIN_ARG_INFO_EX(arginfo_set_multi, 0, 0, 2)
    ZEND_ARG_INFO(0, resource)
    ZEND_ARG_ARRAY_INFO(0, values, 0)
    ZEND_ARG_INFO(0, expiration)
ZEND_END_ARG_INFO()

COUCHBASE_ARG_PREFIX
ZEND_BEGIN_ARG_INFO_EX(arginfo_replace, 0, 0, 3)
    ZEND_ARG_INFO(0, resource)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
    ZEND_ARG_INFO(0, expiration)
    ZEND_ARG_INFO(0, cas)
ZEND_END_ARG_INFO()

COUCHBASE_ARG_PREFIX
ZEND_BEGIN_ARG_INFO_EX(arginfo_append, 0, 0, 3)
    ZEND_ARG_INFO(0, resource)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
    ZEND_ARG_INFO(0, expiration)
    ZEND_ARG_INFO(0, cas)
ZEND_END_ARG_INFO()

COUCHBASE_ARG_PREFIX
ZEND_BEGIN_ARG_INFO_EX(arginfo_prepend, 0, 0, 3)
    ZEND_ARG_INFO(0, resource)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
    ZEND_ARG_INFO(0, expiration)
    ZEND_ARG_INFO(0, cas)
ZEND_END_ARG_INFO()

COUCHBASE_ARG_PREFIX
ZEND_BEGIN_ARG_INFO_EX(arginfo_cas, 0, 0, 4)
    ZEND_ARG_INFO(0, resource)
    ZEND_ARG_INFO(0, cas)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
    ZEND_ARG_INFO(0, expiration)
ZEND_END_ARG_INFO()

COUCHBASE_ARG_PREFIX
ZEND_BEGIN_ARG_INFO_EX(arginfo_get, 0, 0, 2)
    ZEND_ARG_INFO(0, resource)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, cache_cb)
    ZEND_ARG_INFO(1, cas_token)
ZEND_END_ARG_INFO()

COUCHBASE_ARG_PREFIX
ZEND_BEGIN_ARG_INFO_EX(arginfo_get_multi, 0, 0, 2)
    ZEND_ARG_INFO(0, resource)
    ZEND_ARG_ARRAY_INFO(0, keys, 0)
    ZEND_ARG_ARRAY_INFO(1, cas_tokens, 1)
ZEND_END_ARG_INFO()

COUCHBASE_ARG_PREFIX
ZEND_BEGIN_ARG_INFO_EX(arginfo_get_delayed, 0, 0, 2)
    ZEND_ARG_INFO(0, resource)
    ZEND_ARG_ARRAY_INFO(0, keys, 0)
    ZEND_ARG_INFO(0, with_cas)
    ZEND_ARG_INFO(0, cb)
ZEND_END_ARG_INFO()

COUCHBASE_ARG_PREFIX
ZEND_BEGIN_ARG_INFO_EX(arginfo_fetch, 0, 0, 1)
    ZEND_ARG_INFO(0, resource)
ZEND_END_ARG_INFO()

COUCHBASE_ARG_PREFIX
ZEND_BEGIN_ARG_INFO_EX(arginfo_fetch_all, 0, 0, 1)
    ZEND_ARG_INFO(0, resource)
ZEND_END_ARG_INFO()

COUCHBASE_ARG_PREFIX
ZEND_BEGIN_ARG_INFO_EX(arginfo_increment, 0, 0, 2)
    ZEND_ARG_INFO(0, resource)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, offset)
    ZEND_ARG_INFO(0, create)
    ZEND_ARG_INFO(0, expiration)
    ZEND_ARG_INFO(0, initial)
ZEND_END_ARG_INFO()

COUCHBASE_ARG_PREFIX
ZEND_BEGIN_ARG_INFO_EX(arginfo_decrement, 0, 0, 2)
    ZEND_ARG_INFO(0, resource)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, offset)
    ZEND_ARG_INFO(0, create)
    ZEND_ARG_INFO(0, expiration)
    ZEND_ARG_INFO(0, initial)
ZEND_END_ARG_INFO()

COUCHBASE_ARG_PREFIX
ZEND_BEGIN_ARG_INFO_EX(arginfo_delete, 0, 0, 2)
    ZEND_ARG_INFO(0, resource)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, cas)
ZEND_END_ARG_INFO()

COUCHBASE_ARG_PREFIX
ZEND_BEGIN_ARG_INFO_EX(arginfo_get_stats, 0, 0, 1)
    ZEND_ARG_INFO(0, resource)
ZEND_END_ARG_INFO()

COUCHBASE_ARG_PREFIX
ZEND_BEGIN_ARG_INFO_EX(arginfo_flush, 0, 0, 1)
    ZEND_ARG_INFO(0, resource)
ZEND_END_ARG_INFO()

COUCHBASE_ARG_PREFIX
ZEND_BEGIN_ARG_INFO_EX(arginfo_result_code, 0, 0, 1)
    ZEND_ARG_INFO(0, resource)
ZEND_END_ARG_INFO()

COUCHBASE_ARG_PREFIX
ZEND_BEGIN_ARG_INFO_EX(arginfo_version, 0, 0, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ couchbase_functions[]
 */
static zend_function_entry couchbase_functions[] = {
    PHP_FE(couchbase_connect, arginfo_connect)
    PHP_FE(couchbase_add, arginfo_add)
    PHP_FE(couchbase_set, arginfo_set)
    PHP_FE(couchbase_set_multi, arginfo_set_multi)
    PHP_FE(couchbase_replace, arginfo_replace)
    PHP_FE(couchbase_prepend, arginfo_prepend)
    PHP_FE(couchbase_append, arginfo_append)
    PHP_FE(couchbase_cas, arginfo_cas)
    PHP_FE(couchbase_get, arginfo_get)
    PHP_FE(couchbase_get_multi, arginfo_get_multi)
    PHP_FE(couchbase_get_delayed, arginfo_get_delayed)
    PHP_FE(couchbase_fetch, arginfo_fetch)
    PHP_FE(couchbase_fetch_all, arginfo_fetch_all)
    PHP_FE(couchbase_increment, arginfo_increment)
    PHP_FE(couchbase_decrement, arginfo_decrement)
    PHP_FE(couchbase_get_stats, arginfo_get_stats)
    PHP_FE(couchbase_delete, arginfo_delete)
    PHP_FE(couchbase_flush, arginfo_flush)
    PHP_FE(couchbase_get_result_code, arginfo_result_code)
    PHP_FE(couchbase_version, arginfo_version)
    {NULL, NULL, NULL}
};
/* }}} */

/* {{{ couchbase_module_entry
 */
zend_module_entry couchbase_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    "couchbase",
    couchbase_functions,
    PHP_MINIT(couchbase),
    PHP_MSHUTDOWN(couchbase),
    PHP_RINIT(couchbase),        /* Replace with NULL if there's nothing to do at request start */
    PHP_RSHUTDOWN(couchbase),    /* Replace with NULL if there's nothing to do at request end */
    PHP_MINFO(couchbase),
#if ZEND_MODULE_API_NO >= 20010901
    PHP_COUCHBASE_VERSION,
#endif
    PHP_MODULE_GLOBALS(couchbase),
    PHP_GINIT(couchbase),
    NULL,
    NULL,
    STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_COUCHBASE
ZEND_GET_MODULE(couchbase)
#endif

/* {{{ OnUpdateCompressionType
 */
static PHP_INI_MH(OnUpdateCompressionType) {
    if (!new_value) {
        COUCHBASE_G(compression_type) = COMPRESSION_TYPE_FASTLZ;
    } else if (!strcmp(new_value, "fastlz")) {
        COUCHBASE_G(compression_type) = COMPRESSION_TYPE_FASTLZ;
    } else if (!strcmp(new_value, "zlib")) {
        COUCHBASE_G(compression_type) = COMPRESSION_TYPE_ZLIB;
    } else {
        return FAILURE;
    }
    return OnUpdateString(entry, new_value, new_value_length, mh_arg1, mh_arg2, mh_arg3, stage TSRMLS_CC);
}
/* }}} */

/* {{{ OnUpdateSerializer
 */
static PHP_INI_MH(OnUpdateSerializer) {
    if (!new_value) {
        COUCHBASE_G(serializer) = COUCHBASE_SERIALIZER_DEFAULT;
    } else if (!strcmp(new_value, "php")) {
        COUCHBASE_G(serializer) = COUCHBASE_SERIALIZER_PHP;
#ifdef HAVE_JSON_API
    } else if (!strcmp(new_value, "json")) {
        COUCHBASE_G(serializer) = COUCHBASE_SERIALIZER_JSON;
    } else if (!strcmp(new_value, "json_array")) {
        COUCHBASE_G(serializer) = COUCHBASE_SERIALIZER_JSON_ARRAY;
#endif
    } else {
        return FAILURE;
    }

    return OnUpdateString(entry, new_value, new_value_length, mh_arg1, mh_arg2, mh_arg3, stage TSRMLS_CC);
}
/* }}} */

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("couchbase.serializer", "php", PHP_INI_ALL, OnUpdateSerializer, serializer, zend_couchbase_globals, couchbase_globals)
    STD_PHP_INI_ENTRY("couchbase.compression_type", "fastlz", PHP_INI_ALL, OnUpdateCompressionType, compression_type, zend_couchbase_globals, couchbase_globals)
PHP_INI_END()
/* }}} */

static void php_couchbase_error_callback(libcouchbase_t handle, libcouchbase_error_t error, const char *errinfo) /* {{{ */ {
    php_couchbase_ctx *ctx = (php_couchbase_ctx *)libcouchbase_get_cookie(handle);
    /**
     * @FIXME: when connect to a non-couchbase-server port (but the socket is valid)
     * like a apache server, process will be hanged by event_loop
     */
    if (ctx->res->seqno < 0) {
        ctx->res->io->stop_event_loop(ctx->res->io);
    }
}
/* }}} */

/* {{{ static void php_couchbase_get_callback(...)
 */
static void
php_couchbase_get_callback(libcouchbase_t handle,
                            const void *cookie,
                            libcouchbase_error_t error,
                            const void *key, size_t nkey,
                            const void *bytes, size_t nbytes,
                            uint32_t flag, uint64_t cas) {
    zval *retval;
    php_couchbase_ctx *ctx = (php_couchbase_ctx *)cookie;
    php_ignore_value(handle);

    if (--ctx->res->seqno == 0) {
        ctx->res->io->stop_event_loop(ctx->res->io);
    }

    ctx->res->rc = error;

    if (LIBCOUCHBASE_SUCCESS != error && LIBCOUCHBASE_KEY_ENOENT != error) {
        ctx->res->io->stop_event_loop(ctx->res->io);
        return;
    }

    if (ctx->res->async) { /* get_delayed */
        zval *k, *v, *dst;

        MAKE_STD_ZVAL(retval);
        array_init(retval);
        zend_hash_next_index_insert(Z_ARRVAL_P(ctx->rv), (void **)&retval, sizeof(zval *), NULL);

        MAKE_STD_ZVAL(k);
        MAKE_STD_ZVAL(v);
        ZVAL_STRINGL(k, (char *)key, nkey, 1);
        ZVAL_STRINGL(v, (char *)bytes, nbytes, 1);

        zend_hash_add(Z_ARRVAL_P(retval), "key", sizeof("key"), (void **)&k, sizeof(zval *), NULL);
        zend_hash_add(Z_ARRVAL_P(retval), "value", sizeof("value"), (void **)&v, sizeof(zval *), NULL);

        if (ctx->flags) {
            zval *c;
            MAKE_STD_ZVAL(c);
            ZVAL_LONG(c, cas);
            zend_hash_add(Z_ARRVAL_P(retval), "cas", sizeof("cas"), (void **)&c, sizeof(zval *), NULL);
        }

    } else {
        if (LIBCOUCHBASE_KEY_ENOENT == error) {
            ZVAL_NULL(ctx->rv);
            return;
        }

        if (IS_ARRAY == Z_TYPE_P(ctx->rv)) { /* multi get */
            zval *v;
            MAKE_STD_ZVAL(v);
            ZVAL_STRINGL(v, (char *)bytes, nbytes, 1);
            zend_hash_add(Z_ARRVAL_P(ctx->rv), (char *)key, nkey + 1, (void **)&v, sizeof(zval *), NULL);
            if (ctx->cas) {
                zval *c;
                MAKE_STD_ZVAL(c);
                ZVAL_LONG(c, cas);
                zend_hash_add(Z_ARRVAL_P(ctx->cas), (char *)key, nkey + 1, (void **)&c, sizeof(zval *), NULL);
            }
        } else {
            ZVAL_STRINGL(ctx->rv, (char *)bytes, nbytes, 1);
            if (ctx->cas) {
                ZVAL_LONG(ctx->cas, cas);
            }
        }
    }
}
/* }}} */

/* {{{ static void php_couchbase_storage_callback(...)
 */
static void
php_couchbase_storage_callback(libcouchbase_t handle,
                                const void *cookie,
                                libcouchbase_storage_t operation,
                                libcouchbase_error_t error,
                                const void *key, size_t nkey,
                                uint64_t cas) {
    php_couchbase_ctx *ctx = (php_couchbase_ctx *)cookie;
    php_ignore_value(handle);
    php_ignore_value(operation);
    php_ignore_value(key);
    php_ignore_value(nkey);
    php_ignore_value(cas);

    if (--ctx->res->seqno == 0) {
        ctx->res->io->stop_event_loop(ctx->res->io);
    }

    ctx->res->rc = error;
    if (error != LIBCOUCHBASE_SUCCESS && error != LIBCOUCHBASE_AUTH_CONTINUE) {
        if (IS_ARRAY == Z_TYPE_P(ctx->rv)) {
            zval *rv;
            MAKE_STD_ZVAL(rv);
            ZVAL_FALSE(rv);
            zend_hash_update(Z_ARRVAL_P(ctx->rv), (char *)key, nkey + 1, (void **)&rv, sizeof(zval *), NULL);
        }
        return;
    }

    if (IS_ARRAY == Z_TYPE_P(ctx->rv)) {
        zval *rv;
        MAKE_STD_ZVAL(rv);
        ZVAL_LONG(rv, cas);
        zend_hash_update(Z_ARRVAL_P(ctx->rv), (char *)key, nkey + 1, (void **)&rv, sizeof(zval *), NULL);
    } else {
        ZVAL_LONG(ctx->rv, cas);
    }
}
/* }}} */

/* {{{ static void php_couchbase_remove_callback(...) */
static void
php_couchbase_remove_callback(libcouchbase_t handle,
                            const void *cookie,
                            libcouchbase_error_t error,
                            const void *key, size_t nkey) {
    php_couchbase_ctx *ctx = (php_couchbase_ctx *)cookie;
    php_ignore_value(handle);
    php_ignore_value(key);
    php_ignore_value(nkey);

    if (--ctx->res->seqno == 0) {
        ctx->res->io->stop_event_loop(ctx->res->io);
    }

    ctx->res->rc = error;
}
/* }}} */

/* {{{ static void php_couchbase_flush_callback(...) */
static void
php_couchbase_flush_callback(libcouchbase_t handle,
                            const void* cookie,
                            const char* server_endpoint,
                            libcouchbase_error_t error) {
    php_couchbase_ctx *ctx = (php_couchbase_ctx *)cookie;
    php_ignore_value(handle);

    if (--ctx->res->seqno == 0) {
        ctx->res->io->stop_event_loop(ctx->res->io);
    }

    if (server_endpoint) {
        ctx->extended_value = (void *)estrdup(server_endpoint);
    }
    ctx->res->rc = error;
}
/* }}} */

/* {{{ static void php_couchbase_arithmetic_callback(...) */
static void
php_couchbase_arithmetic_callback(libcouchbase_t handle,
                                const void *cookie,
                                libcouchbase_error_t error,
                                const void *key, size_t nkey,
                                uint64_t value, uint64_t cas) {
    php_couchbase_ctx *ctx = (php_couchbase_ctx *)cookie;
    php_ignore_value(handle);
    php_ignore_value(key);
    php_ignore_value(nkey);
    php_ignore_value(cas);

    if (--ctx->res->seqno == 0) {
        ctx->res->io->stop_event_loop(ctx->res->io);
    }

    ctx->res->rc = error;
    if (LIBCOUCHBASE_SUCCESS != error) {
        return;
    }

    ZVAL_LONG(ctx->rv, value);
}
/* }}} */

/* {{{ static void php_couchbase_stat_callback(...) */
static void
php_couchbase_stat_callback(libcouchbase_t handle,
                            const void* cookie,
                            const char* server_endpoint,
                            libcouchbase_error_t error,
                            const void* key, size_t nkey,
                            const void* bytes, size_t nbytes) {
    php_couchbase_ctx *ctx = (php_couchbase_ctx *)cookie;
    php_ignore_value(handle);

    ctx->res->rc = error;
    if (LIBCOUCHBASE_SUCCESS != error || nkey == 0) {
        --ctx->res->seqno;
        ctx->res->io->stop_event_loop(ctx->res->io);
        return;
    } else if (nkey > 0) {
        zval *node, *v;
        zval **ppzval;
        if (IS_ARRAY != Z_TYPE_P(ctx->rv)) {
            array_init(ctx->rv);
        }

        if (zend_hash_find(Z_ARRVAL_P(ctx->rv), (char *)server_endpoint, strlen(server_endpoint) + 1, (void **)&ppzval) == SUCCESS) {
            node = *ppzval;
        } else {
            MAKE_STD_ZVAL(node);
            array_init(node);
            zend_hash_add(Z_ARRVAL_P(ctx->rv), (char *)server_endpoint, strlen(server_endpoint) + 1, (void **)&node, sizeof(zval *), NULL);
        }

        MAKE_STD_ZVAL(v);
        ZVAL_STRINGL(v, (char *)bytes, nbytes, 1);
        zend_hash_add(Z_ARRVAL_P(node), (char *)key, nkey + 1, (void **)&v, sizeof(zval *), NULL);
    }

}
/* }}} */

static void php_couchbase_res_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC) /* {{{ */ {
    php_couchbase_res *couchbase_res = (php_couchbase_res*)rsrc->ptr;
    if (couchbase_res) {
        if (couchbase_res->handle) {
            libcouchbase_destroy(couchbase_res->handle);
        }
        efree(couchbase_res);
    }
}
/* }}} */

static void php_couchbase_get_impl(INTERNAL_FUNCTION_PARAMETERS TSRMLS_DC, int multi) /* {{{ */ {
    char *key, **keys;
    long *klens, klen = 0;
    int  nkey, flag = 0;
    zval *res, *cas_token = NULL;
#if PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION > 2
    zend_fcall_info fci = {0};
    zend_fcall_info_cache fci_cache;
#else
    zval *callback = NULL;
#endif
    if (multi) {
        zval *akeys;
        zval **ppzval;
        int i;
        if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra|zl", &res, &akeys, &cas_token, &flag) == FAILURE) {
            return;
        }

        array_init(return_value);

        nkey = zend_hash_num_elements(Z_ARRVAL_P(akeys));
        keys = ecalloc(nkey, sizeof(char *));
        klens = ecalloc(nkey, sizeof(long));

        for(i=0, zend_hash_internal_pointer_reset(Z_ARRVAL_P(akeys));
                zend_hash_has_more_elements(Z_ARRVAL_P(akeys)) == SUCCESS;
                zend_hash_move_forward(Z_ARRVAL_P(akeys)), i++) {
            if (zend_hash_get_current_data(Z_ARRVAL_P(akeys), (void**)&ppzval) == FAILURE) {
                nkey--;
                continue;
            }

            if (IS_ARRAY != Z_TYPE_PP(ppzval)) {
                convert_to_string_ex(ppzval);
            }

            if (!Z_STRLEN_PP(ppzval)) {
                nkey--;
                continue;
            }

            keys[i] = Z_STRVAL_PP(ppzval);
            klens[i] = Z_STRLEN_PP(ppzval);
        }

        if (!nkey) {
            efree(keys);
            efree(klens);
            return;
        }

        if (cas_token && IS_ARRAY != Z_TYPE_P(cas_token)) {
            array_init(cas_token);
        }
    } else {
#if PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION > 2
        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|fz", &res, &key, &klen, &fci, &fci_cache, &cas_token) == FAILURE)
#else
        if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|zz", &res, &key, &klen, &callback, &cas_token) == FAILURE)
#endif
        {
            return;
        }
#if PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION < 3
        if (callback && Z_TYPE_P(callback) != IS_NULL && !zend_is_callable(callback, 0, NULL)) {
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "Third argument is expected to be a valid callback");
            return;
        }
#endif
        if (!klen) {
            return;
        }
        nkey = 1;
        keys = &key;
        klens = &klen;
    }
    {
        libcouchbase_error_t retval;
        php_couchbase_res *couchbase_res;
        php_couchbase_ctx *ctx;

        ZEND_FETCH_RESOURCE(couchbase_res, php_couchbase_res *, &res, -1, PHP_COUCHBASE_RESOURCE, le_couchbase);
        if (couchbase_res->async) {
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "There are some results should be fetched before do any sync request");
            RETURN_FALSE;
        }

        ctx = ecalloc(1, sizeof(php_couchbase_ctx));
        ctx->res = couchbase_res;
        ctx->rv  = return_value;
        ctx->cas = cas_token;

        retval = libcouchbase_mget(couchbase_res->handle, (const void *)ctx, nkey, (const void *)keys, (size_t *)klens, NULL);
        if (LIBCOUCHBASE_SUCCESS != retval) {
            if (multi) {
                efree(keys);
                efree(klens);
            }
            efree(ctx);
            php_error_docref(NULL TSRMLS_CC, E_WARNING,
                    "Failed to schedule get request: %s", libcouchbase_strerror(couchbase_res->handle, retval));
            RETURN_FALSE;
        }

        couchbase_res->seqno += nkey;
        couchbase_res->io->run_event_loop(couchbase_res->io);
        if (LIBCOUCHBASE_SUCCESS != ctx->res->rc) {
            if (LIBCOUCHBASE_KEY_ENOENT == ctx->res->rc) {
#if PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION > 2
                if (fci.size)
#else
                if (callback)
#endif
                {
                    zval *retval_ptr, *result, *zkey;
                    zval **params[3];

                    MAKE_STD_ZVAL(result);
                    MAKE_STD_ZVAL(zkey);
                    ZVAL_STRINGL(zkey, key, klen, 0);
                    params[0] = &res;
                    params[1] = &zkey;
                    params[2] = &result;
#if PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION > 2
                    fci.retval_ptr_ptr = &retval_ptr;
                    fci.param_count = 3;
                    fci.params = params;
                    if (zend_call_function(&fci, &fci_cache TSRMLS_CC) == SUCCESS && fci.retval_ptr_ptr && *fci.retval_ptr_ptr) {
                        if (Z_TYPE_P(retval_ptr) == IS_BOOL && Z_BVAL_P(retval_ptr)) {
                            zval_ptr_dtor(&zkey);
                            RETURN_ZVAL(result, 0, 0);
                        }
                    }
#else
                    if (call_user_function_ex(EG(function_table), NULL, callback, &retval_ptr, 3, params, 0, NULL TSRMLS_CC) == SUCCESS) {
                        if (Z_TYPE_P(retval_ptr) == IS_BOOL && Z_BVAL_P(retval_ptr)) {
                            zval_ptr_dtor(&zkey);
                            RETURN_ZVAL(result, 0, 0);
                        }
                    }
#endif
                    zval_ptr_dtor(&zkey);
                    zval_ptr_dtor(&result);
                }
            } else {
                php_error_docref(NULL TSRMLS_CC, E_WARNING,
                        "Faild to get a value from server: %s", libcouchbase_strerror(couchbase_res->handle, ctx->res->rc));
            }
        }
        if (multi) {
            efree(keys);
            efree(klens);
        }
        efree(ctx);
    }
}
/* }}} */

static void php_couchbase_get_delayed_impl(INTERNAL_FUNCTION_PARAMETERS TSRMLS_DC) /* {{{ */ {
    zval *res, *akeys;
    long with_cas = 0;
#if PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION > 2
    zend_fcall_info fci = {0};
    zend_fcall_info_cache fci_cache;
#else
    zval *callback = NULL;
#endif

#if PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION > 2
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra|lf", &res, &akeys, &with_cas, &fci, &fci_cache) == FAILURE)  {
        return;
    }
#else
    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra|lz", &res, &akeys, &with_cas, &callback) == FAILURE) {
        return;
    } else if (callback && Z_TYPE_P(callback) != IS_NULL && !zend_is_callable(callback, 0, NULL)) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Third argument is expected to be a valid callback");
        return;
    }
#endif
    else {
        zval **ppzval;
        libcouchbase_error_t retval;
        php_couchbase_res *couchbase_res;
        php_couchbase_ctx *ctx;
        char **keys;
        long nkey, *klens, i;

        nkey = zend_hash_num_elements(Z_ARRVAL_P(akeys));
        keys = ecalloc(nkey, sizeof(char *));
        klens = ecalloc(nkey, sizeof(long));

        for(i=0, zend_hash_internal_pointer_reset(Z_ARRVAL_P(akeys));
                zend_hash_has_more_elements(Z_ARRVAL_P(akeys)) == SUCCESS;
                zend_hash_move_forward(Z_ARRVAL_P(akeys)), i++) {
            if (zend_hash_get_current_data(Z_ARRVAL_P(akeys), (void**)&ppzval) == FAILURE) {
                nkey--;
                continue;
            }

            if (IS_ARRAY != Z_TYPE_PP(ppzval)) {
                convert_to_string_ex(ppzval);
            }

            if (!Z_STRLEN_PP(ppzval)) {
                nkey--;
                continue;
            }

            keys[i] = Z_STRVAL_PP(ppzval);
            klens[i] = Z_STRLEN_PP(ppzval);
        }

        if (!nkey) {
            efree(keys);
            efree(klens);
            return;
        }

        ZEND_FETCH_RESOURCE(couchbase_res, php_couchbase_res *, &res, -1, PHP_COUCHBASE_RESOURCE, le_couchbase);

        ctx = ecalloc(1, sizeof(php_couchbase_ctx));
        ctx->res = couchbase_res;
        ctx->flags = with_cas;
        couchbase_res->seqno += nkey;

        retval = libcouchbase_mget(couchbase_res->handle, (const void *)ctx, nkey, (const void * const *)keys, (size_t *)klens, NULL);
        if (LIBCOUCHBASE_SUCCESS != retval) {
            efree(keys);
            efree(klens);
            efree(ctx);
            php_error_docref(NULL TSRMLS_CC, E_WARNING,
                    "Failed to schedule delayed get request: %s", libcouchbase_strerror(couchbase_res->handle, retval));
            RETURN_FALSE;
        }
        couchbase_res->async = 1;
        libcouchbase_set_cookie(couchbase_res->handle, (const void *)ctx);
        efree(keys);
        efree(klens);
        if (
#if PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION > 2
                fci.sizex
#else
                callback
#endif
           ) {
            zval *result, **ppzval, *retval_ptr;
            zval **params[2];

            MAKE_STD_ZVAL(result);
            array_init(result);
            ctx->rv = result;   
            couchbase_res->io->run_event_loop(couchbase_res->io);
            couchbase_res->async = 0;
            for(zend_hash_internal_pointer_reset(Z_ARRVAL_P(result));
                    zend_hash_has_more_elements(Z_ARRVAL_P(result)) == SUCCESS;
                    zend_hash_move_forward(Z_ARRVAL_P(result))) {
                if (zend_hash_get_current_data(Z_ARRVAL_P(result), (void**)&ppzval) == FAILURE) {
                    continue;
                }
                params[0] = &res;
                params[1] = ppzval;
#if PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION > 2
                fci.retval_ptr_ptr = &retval_ptr;
                fci.param_count = 2;
                fci.params = params;
                zend_call_function(&fci, &fci_cache TSRMLS_CC);
#else
                call_user_function_ex(EG(function_table), NULL, callback, &retval_ptr, 2, params, 0, NULL TSRMLS_CC);
#endif
                zval_ptr_dtor(&retval_ptr);
            }
            zval_ptr_dtor(&result);
            efree(ctx);
        }
    }
    RETURN_TRUE;
}
/* }}} */

static void php_couchbase_fetch_impl(INTERNAL_FUNCTION_PARAMETERS TSRMLS_DC, int multi) /* {{{ */ {
    zval *res;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    } else {
        libcouchbase_error_t retval;
        php_couchbase_res *couchbase_res;
        php_couchbase_ctx *ctx;

        ZEND_FETCH_RESOURCE(couchbase_res, php_couchbase_res *, &res, -1, PHP_COUCHBASE_RESOURCE, le_couchbase);
        if (!couchbase_res->async) {
            RETURN_FALSE;
        }
        ctx = (php_couchbase_ctx *)libcouchbase_get_cookie(couchbase_res->handle);
        if (couchbase_res->async == 2) {
fetch_one:
            {
                char *key;
                int key_len;
                long index = 0;
                zval **ppzval;
                zval *stash = (zval *)ctx->extended_value;
                if (zend_hash_num_elements(Z_ARRVAL_P(stash)) == 0) {
                    couchbase_res->async = 0;
                    zval_ptr_dtor(&stash);
                    RETURN_NULL();
                }
                zend_hash_internal_pointer_reset(Z_ARRVAL_P(stash));
                zend_hash_get_current_data(Z_ARRVAL_P(stash), (void **)&ppzval);
                RETVAL_ZVAL(*ppzval, 1, 0);
                zend_hash_get_current_key_ex(Z_ARRVAL_P(stash), &key, &key_len, &index, 0, NULL);
                zend_hash_index_del(Z_ARRVAL_P(stash), index);
                return;
            }
        }

        ctx->rv = return_value;
        array_init(return_value);

        couchbase_res->io->run_event_loop(couchbase_res->io);
        if (!multi) {
            zval *stash;
            MAKE_STD_ZVAL(stash);
            ZVAL_ZVAL(stash, return_value, 1, 0);
            zval_dtor(return_value);
            ctx->extended_value = (void *)stash;
            couchbase_res->async = 2;
            goto fetch_one;
        } else {
            efree(ctx);
            couchbase_res->async = 0;
        } 
    }
}
/* }}} */

static void php_couchbase_store_impl(INTERNAL_FUNCTION_PARAMETERS, libcouchbase_storage_t op, int multi) /* {{{ */ {
    zval *res;
    libcouchbase_error_t retval;
    php_couchbase_res *couchbase_res;
    php_couchbase_ctx *ctx;
    time_t exp = {0};
    long expire = 0, cas = 0;

    if (!multi) {
        char *key;
        zval *value;
        long klen = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsz|ll", &res, &key, &klen, &value, &expire, &cas) == FAILURE) {
            return;
        }

        ZEND_FETCH_RESOURCE(couchbase_res, php_couchbase_res *, &res, -1, PHP_COUCHBASE_RESOURCE, le_couchbase);
        if (couchbase_res->async) {
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "There are some results should be fetched before do any sync request");
            RETURN_FALSE;
        }

        if (!klen) {
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to schedule set request: Empty key");
            RETURN_FALSE;
        }

        if (IS_STRING != Z_TYPE_P(value)) {
            convert_to_string_ex(&value);
        }


        ctx = ecalloc(1, sizeof(php_couchbase_ctx));
        ctx->res = couchbase_res;
        ctx->rv  = return_value;
        couchbase_res->seqno += 1;

        if (expire) {
            exp = expire;
        }

        retval = libcouchbase_store(couchbase_res->handle,
                (const void *)ctx, op, key, klen, Z_STRVAL_P(value), Z_STRLEN_P(value), 0, exp, (uint64_t)cas);
        if (LIBCOUCHBASE_SUCCESS != retval) {
            efree(ctx);
            php_error_docref(NULL TSRMLS_CC, E_WARNING,
                    "Failed to schedule set request: %s", libcouchbase_strerror(couchbase_res->handle, retval));
            RETURN_FALSE;
        }

    } else {
        zval *akeys, **ppzval;
        char *key;
        long klen = 0, idx;
        int key_type, nkey = 0;

        if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra|l", &res, &akeys, &expire) == FAILURE) {
            return;
        }

        ZEND_FETCH_RESOURCE(couchbase_res, php_couchbase_res *, &res, -1, PHP_COUCHBASE_RESOURCE, le_couchbase);
        if (couchbase_res->async) {
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "There are some results should be fetched before do any sync request");
            RETURN_FALSE;
        }

        ctx = ecalloc(1, sizeof(php_couchbase_ctx));
        ctx->res = couchbase_res;
        ctx->rv  = return_value;
        array_init(ctx->rv);

        if (expire) {
            exp = expire;
        }

        for(zend_hash_internal_pointer_reset(Z_ARRVAL_P(akeys));
                zend_hash_has_more_elements(Z_ARRVAL_P(akeys)) == SUCCESS;
                zend_hash_move_forward(Z_ARRVAL_P(akeys))) {
            if (zend_hash_get_current_data(Z_ARRVAL_P(akeys), (void**)&ppzval) == FAILURE) {
                continue;
            }
            switch((key_type = zend_hash_get_current_key(Z_ARRVAL_P(akeys), &key, &idx, 0))) {
                case HASH_KEY_IS_LONG:
                    spprintf(&key, 0, "%ld", idx);
                    break;
                case HASH_KEY_IS_STRING:
                    break;
                default:
                    continue;
            }

            if(!(klen = strlen(key))) {
                continue;
            }

            if (IS_ARRAY != Z_TYPE_PP(ppzval)) {
                convert_to_string_ex(ppzval);
            }

            retval = libcouchbase_store(couchbase_res->handle,
                    (const void *)ctx, op, key, klen, Z_STRVAL_PP(ppzval), Z_STRLEN_PP(ppzval), 0, exp, 0);
            if (LIBCOUCHBASE_SUCCESS != retval) {
                if(HASH_KEY_IS_LONG == key_type) {
                    efree(key);
                }
                efree(ctx);
                php_error_docref(NULL TSRMLS_CC, E_WARNING,
                        "Failed to schedule set request: %s", libcouchbase_strerror(couchbase_res->handle, retval));
                RETURN_FALSE;
            }

            if (HASH_KEY_IS_LONG == key_type) {
                efree(key);
            }
            nkey++;
        }
        if (!nkey) {
            efree(ctx);
            return;
        }
        couchbase_res->seqno += nkey;
    }
    {
        couchbase_res->io->run_event_loop(couchbase_res->io);
        if (IS_ARRAY != Z_TYPE_P(return_value)) {
            if (LIBCOUCHBASE_SUCCESS != ctx->res->rc) {
                RETVAL_FALSE;
                switch (op) {
                    case LIBCOUCHBASE_ADD:
                        if (LIBCOUCHBASE_KEY_EEXISTS == ctx->res->rc) {
                            break;
                        }
                    case LIBCOUCHBASE_APPEND:
                    case LIBCOUCHBASE_PREPEND:
                        if (LIBCOUCHBASE_NOT_STORED == ctx->res->rc) {
                            break;
                        }
                    case LIBCOUCHBASE_REPLACE:
                    case LIBCOUCHBASE_SET:
                        if (LIBCOUCHBASE_KEY_ENOENT == ctx->res->rc) {
                            break;
                        }
                        if (cas && LIBCOUCHBASE_KEY_EEXISTS == ctx->res->rc) {
                            break;
                        }
                    default:
                        php_error_docref(NULL TSRMLS_CC, E_WARNING,
                                "Faild to store a value to server: %s", libcouchbase_strerror(couchbase_res->handle, ctx->res->rc));
                        break;
                }
            }
        }

        efree(ctx);
    }
}
/* }}} */

static void php_couchbase_remove_impl(INTERNAL_FUNCTION_PARAMETERS TSRMLS_DC) /* {{{ */ {
    char *key;
    zval *res;
    long klen = 0, cas = 0;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|l", &res, &key, &klen, &cas) == FAILURE) {
        return;
    } else {
        libcouchbase_error_t retval;
        php_couchbase_res *couchbase_res;
        php_couchbase_ctx *ctx;

        ZEND_FETCH_RESOURCE(couchbase_res, php_couchbase_res *, &res, -1, PHP_COUCHBASE_RESOURCE, le_couchbase);
        if (couchbase_res->async) {
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "There are some results should be fetched before do any sync request");
            RETURN_FALSE;
        }

        ctx = ecalloc(1, sizeof(php_couchbase_ctx));
        ctx->res = couchbase_res;

        retval = libcouchbase_remove(couchbase_res->handle, (const void *)ctx, (const void * const *)key, klen, cas);
        if (LIBCOUCHBASE_SUCCESS != retval) {
            efree(ctx);
            php_error_docref(NULL TSRMLS_CC, E_WARNING,
                    "Failed to schedule delete request: %s", libcouchbase_strerror(couchbase_res->handle, retval));
            RETURN_FALSE;
        }

        couchbase_res->seqno += 1;
        couchbase_res->io->run_event_loop(couchbase_res->io);
        if (LIBCOUCHBASE_SUCCESS != ctx->res->rc && LIBCOUCHBASE_KEY_ENOENT != ctx->res->rc) {
            php_error_docref(NULL TSRMLS_CC, E_WARNING,
                    "Faild to remove a value from server: %s", libcouchbase_strerror(couchbase_res->handle, ctx->res->rc));
            RETVAL_FALSE;
        } else if (LIBCOUCHBASE_KEY_ENOENT == ctx->res->rc) {
            RETVAL_FALSE;
        } else {
            RETURN_TRUE;
        }
        efree(ctx);
    }
}
/* }}} */

static void php_couchbase_flush_impl(INTERNAL_FUNCTION_PARAMETERS TSRMLS_DC) /* {{{ */ {
    zval *res;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    } else {
        libcouchbase_error_t retval;
        php_couchbase_res *couchbase_res;
        php_couchbase_ctx *ctx;

        ZEND_FETCH_RESOURCE(couchbase_res, php_couchbase_res *, &res, -1, PHP_COUCHBASE_RESOURCE, le_couchbase);
        if (couchbase_res->async) {
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "There are some results should be fetched before do any sync request");
            RETURN_FALSE;
        }

        ctx = ecalloc(1, sizeof(php_couchbase_ctx));
        ctx->res = couchbase_res;

        retval = libcouchbase_flush(couchbase_res->handle, (const void *)ctx);
        if (LIBCOUCHBASE_SUCCESS != retval) {
            efree(ctx);
            php_error_docref(NULL TSRMLS_CC, E_WARNING,
                    "Failed to schedule flush request: %s", libcouchbase_strerror(couchbase_res->handle, retval));
            RETURN_FALSE;
        }

        couchbase_res->seqno += 1;
        couchbase_res->io->run_event_loop(couchbase_res->io);
        if (LIBCOUCHBASE_SUCCESS != ctx->res->rc) {
            efree(ctx);
            php_error_docref(NULL TSRMLS_CC, E_WARNING,
                    "Faild to flush node %s: %s", ctx->extended_value?(char *)ctx->extended_value : "", libcouchbase_strerror(couchbase_res->handle, ctx->res->rc));
            RETURN_FALSE;
        }
        efree(ctx);
    }
    RETURN_TRUE;
}
/* }}} */

static void php_couchbase_arithmetic_impl(INTERNAL_FUNCTION_PARAMETERS, char op) /* {{{ */ {
    zval *res;
    char *key;
    time_t exp = {0};
    long klen = 0, offset = 1, expire = 0;
    long create = 0, initial = 0;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|llll", &res, &key, &klen, &offset, &create, &expire, &initial) == FAILURE) {
        return;
    } else {
        libcouchbase_error_t retval;
        php_couchbase_res *couchbase_res;
        php_couchbase_ctx *ctx;
        long delta = (op == '+')? offset : -offset;

        ZEND_FETCH_RESOURCE(couchbase_res, php_couchbase_res *, &res, -1, PHP_COUCHBASE_RESOURCE, le_couchbase);
        if (couchbase_res->async) {
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "There are some results should be fetched before do any sync request");
            RETURN_FALSE;
        }

        if (expire) {
            exp = expire;
        }

        ctx = ecalloc(1, sizeof(php_couchbase_ctx));
        ctx->res = couchbase_res;
        ctx->rv = return_value;

        retval = libcouchbase_arithmetic(couchbase_res->handle, (const void *)ctx, (const void * const *)key, klen, delta, exp, create, initial);
        if (LIBCOUCHBASE_SUCCESS != retval) {
            efree(ctx);
            php_error_docref(NULL TSRMLS_CC, E_WARNING,
                    "Failed to schedule rithmetic request: %s", libcouchbase_strerror(couchbase_res->handle, retval));
            RETURN_FALSE;
        }

        couchbase_res->seqno += 1;
        couchbase_res->io->run_event_loop(couchbase_res->io);
        if (LIBCOUCHBASE_SUCCESS != ctx->res->rc) {
            efree(ctx);
            php_error_docref(NULL TSRMLS_CC, E_WARNING,
                    "Faild to %s value in server: %s", (op == '+')? "increment" : "decrement", libcouchbase_strerror(couchbase_res->handle, ctx->res->rc));
            RETURN_FALSE;
        }
        efree(ctx);
    }
}
/* }}} */

static void php_couchbase_stats_impl(INTERNAL_FUNCTION_PARAMETERS) /* {{{ */ {
    zval *res;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    } else {
        libcouchbase_error_t retval;
        php_couchbase_res *couchbase_res;
        php_couchbase_ctx *ctx;

        ZEND_FETCH_RESOURCE(couchbase_res, php_couchbase_res *, &res, -1, PHP_COUCHBASE_RESOURCE, le_couchbase);
        if (couchbase_res->async) {
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "There are some results should be fetched before do any sync request");
            RETURN_FALSE;
        }

        ctx = ecalloc(1, sizeof(php_couchbase_ctx));
        ctx->res = couchbase_res;
        ctx->rv = return_value;

        retval = libcouchbase_server_stats(couchbase_res->handle, (const void *)ctx, NULL, 0);
        if (LIBCOUCHBASE_SUCCESS != retval) {
            efree(ctx);
            php_error_docref(NULL TSRMLS_CC, E_WARNING,
                    "Failed to schedule stat request: %s", libcouchbase_strerror(couchbase_res->handle, retval));
            RETURN_FALSE;
        }

        couchbase_res->seqno += 1;
        couchbase_res->io->run_event_loop(couchbase_res->io);
        if (LIBCOUCHBASE_SUCCESS != ctx->res->rc) {
            efree(ctx);
            php_error_docref(NULL TSRMLS_CC, E_WARNING,
                    "Faild to stat: %s", libcouchbase_strerror(couchbase_res->handle, ctx->res->rc));
            RETURN_FALSE;
        }
        efree(ctx);
    }
}
/* }}} */

static void php_couchbase_cas_impl(INTERNAL_FUNCTION_PARAMETERS) /* {{{ */ {
    char *key;
    zval *res, *value;
    long klen = 0, expire = 0, cas = 0;
    time_t exp = {0};

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rlsz|l", &res, &cas, &key, &klen, &value, &expire) == FAILURE) {
        return;
    } else {
        libcouchbase_error_t retval;
        php_couchbase_res *couchbase_res;
        php_couchbase_ctx *ctx;

        ZEND_FETCH_RESOURCE(couchbase_res, php_couchbase_res *, &res, -1, PHP_COUCHBASE_RESOURCE, le_couchbase);
        if (couchbase_res->async) {
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "There are some results should be fetched before do any sync request");
            RETURN_FALSE;
        }

        ctx = ecalloc(1, sizeof(php_couchbase_ctx));
        ctx->res = couchbase_res;
        ctx->rv = return_value;

        if (expire) {
            exp = expire;
        }

        retval = libcouchbase_store(couchbase_res->handle, (const void *)ctx,
                LIBCOUCHBASE_SET, key, klen, Z_STRVAL_P(value), Z_STRLEN_P(value), 0, exp, (uint64_t)cas);
        if (LIBCOUCHBASE_SUCCESS != retval) {
            efree(ctx);
            php_error_docref(NULL TSRMLS_CC, E_WARNING,
                    "Failed to schedule cas request: %s", libcouchbase_strerror(couchbase_res->handle, retval));
            RETURN_FALSE;
        }

        ++couchbase_res->seqno;
        couchbase_res->io->run_event_loop(couchbase_res->io);
        if (LIBCOUCHBASE_SUCCESS == ctx->res->rc) {
            ZVAL_TRUE(return_value);
        } else if (LIBCOUCHBASE_KEY_EEXISTS == ctx->res->rc) {
            ZVAL_FALSE(return_value);
        } else {
            php_error_docref(NULL TSRMLS_CC, E_WARNING,
                    "Faild to store a value to server: %s", libcouchbase_strerror(couchbase_res->handle, ctx->res->rc));
        }

        efree(ctx);
    }
}
/* }}} */

/* {{{ proto couchbase_connect(string $host[, string $user[, string $password[, string $bucket]]])
*/
PHP_FUNCTION(couchbase_connect) {
    char *host;
    char *user = NULL, *passwd = NULL, *bucket = NULL;
    int host_len, user_len, passwd_len, bucket_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|sss",
                &host, &host_len, &user, &user_len, &passwd, &passwd_len, &bucket, &bucket_len) == FAILURE) {
        return;
    } else {
        libcouchbase_t handle;
        libcouchbase_error_t retval;
        libcouchbase_io_opt_t *iops;
        php_couchbase_res *couchbase_res;
        php_couchbase_ctx *ctx;

        iops = libcouchbase_create_io_ops(LIBCOUCHBASE_IO_OPS_DEFAULT, NULL, NULL);
        if (!iops) {
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to create IO instance");
            RETURN_FALSE;
        }

        if (!bucket) {
            bucket = "default";
        }

        handle = libcouchbase_create(host, user, passwd, bucket, iops);
        if (!handle) {
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to create libcouchbase instance");
            RETURN_FALSE;
        }

        php_ignore_value(libcouchbase_set_error_callback(handle, php_couchbase_error_callback));

        if (LIBCOUCHBASE_SUCCESS != (retval = libcouchbase_connect(handle))) {
            php_error_docref(NULL TSRMLS_CC, E_WARNING,
                    "Failed to connect libcouchbase to server: %s", libcouchbase_strerror(handle, retval));
            RETURN_FALSE;
        }

        php_ignore_value(libcouchbase_set_get_callback(handle, php_couchbase_get_callback));
        php_ignore_value(libcouchbase_set_storage_callback(handle, php_couchbase_storage_callback));
        php_ignore_value(libcouchbase_set_remove_callback(handle, php_couchbase_remove_callback));
        php_ignore_value(libcouchbase_set_flush_callback(handle, php_couchbase_flush_callback));
        php_ignore_value(libcouchbase_set_arithmetic_callback(handle, php_couchbase_arithmetic_callback));
        php_ignore_value(libcouchbase_set_stat_callback(handle, php_couchbase_stat_callback));

        couchbase_res = emalloc(sizeof(php_couchbase_res));
        couchbase_res->handle = handle;
        couchbase_res->seqno = -1; /* tell error callback stop event loop when error occurred */
        couchbase_res->io = iops;
        couchbase_res->async = 0;

        ctx = ecalloc(1, sizeof(php_couchbase_ctx));
        ctx->res = couchbase_res;
        libcouchbase_set_cookie(handle, (const void *)ctx);

        /* wait for the connection established */
        libcouchbase_wait(handle);

        couchbase_res->seqno = 0;
        if (LIBCOUCHBASE_SUCCESS != (retval = libcouchbase_get_last_error(handle))) {
            php_error_docref(NULL TSRMLS_CC, E_WARNING,
                    "Failed to connect libcouchbase to server: %s", libcouchbase_strerror(handle, retval));
            libcouchbase_destroy(handle);
            efree(couchbase_res);
            RETURN_FALSE;
        }

        ZEND_REGISTER_RESOURCE(return_value, couchbase_res, le_couchbase);
    }
}
/* }}} */

/* {{{ proto couchbase_get(resource $couchbase, string $key[, callback $cache_cb[, float &$cas_tokey]])
 */
PHP_FUNCTION(couchbase_get) {
    php_couchbase_get_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto couchbase_get_multi(resource $couchbase, array $keys[, array &cas[, int $flag]])
 */
PHP_FUNCTION(couchbase_get_multi) {
    php_couchbase_get_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto couchbase_cas(resource $couchbase, float $cas, string $key, mixed $value[, int $expiration])
 */
PHP_FUNCTION(couchbase_cas) {
    php_couchbase_cas_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto couchbase_add(resource $couchbase, string $key, mixed $value[, int $expiration])
 */
PHP_FUNCTION(couchbase_add) {
    php_couchbase_store_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, LIBCOUCHBASE_ADD, 0);
}
/* }}} */

/* {{{ proto couchbase_set(resource $couchbase, string $key, mixed $value[, int $expiration])
 */
PHP_FUNCTION(couchbase_set) {
    php_couchbase_store_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, LIBCOUCHBASE_SET, 0);
}
/* }}} */

/* {{{ proto couchbase_set_multi(resource $couchbase, array $values[, int $expiration])
 */
PHP_FUNCTION(couchbase_set_multi) {
    php_couchbase_store_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, LIBCOUCHBASE_SET, 1);
}
/* }}} */

/* {{{ proto couchbase_prepend(resource $couchbase, string $key[, long $cas = 0])
 */
PHP_FUNCTION(couchbase_prepend) {
    php_couchbase_store_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, LIBCOUCHBASE_PREPEND, 0);
}
/* }}} */

/* {{{ proto couchbase_append(resource $couchbase, string $key[, long $cas = 0])
 */
PHP_FUNCTION(couchbase_append) {
    php_couchbase_store_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, LIBCOUCHBASE_APPEND, 0);
}
/* }}} */

/* {{{ proto couchbase_replace(resource $couchbase, string $key, mixed $value[, int $expiration[, long $cas = 0]])
 */
PHP_FUNCTION(couchbase_replace) {
    php_couchbase_store_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, LIBCOUCHBASE_REPLACE, 0);
}
/* }}} */

/* {{{ proto couchbase_increment(resource $couchbase, string $key[, int $offset = 1[, bool $create = false[, int $expiration = 0[, int $initial = 0]]]])
 */
PHP_FUNCTION(couchbase_increment) {
    php_couchbase_arithmetic_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, '+');
}
/* }}} */

/* {{{ proto couchbase_decrement(resource $couchbase, string $key[, int $offset = 1[, bool $create = false[, int $expiration = 0[, int $initial = 0]]]])
 */
PHP_FUNCTION(couchbase_decrement) {
    php_couchbase_arithmetic_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, '-');
}
/* }}} */

/* {{{ proto couchbase_get_stats(resource $couchbase)
 */
PHP_FUNCTION(couchbase_get_stats) {
    php_couchbase_stats_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto couchbase_get_delayed(resource $couchbase, array $keys[, bool $with_cas[, callback $value_cb]])
 */
PHP_FUNCTION(couchbase_get_delayed) {
    php_couchbase_get_delayed_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto couchbase_fetch(resource $couchbase)
 */
PHP_FUNCTION(couchbase_fetch) {
    php_couchbase_fetch_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto couchbase_fetch_all(resource $couchbase)
 */
PHP_FUNCTION(couchbase_fetch_all) {
    php_couchbase_fetch_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto couchbase_delete(resource $couchbase, string $key[, long $cas = 0])
 */
PHP_FUNCTION(couchbase_delete) {
    php_couchbase_remove_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto couchbase_flush(resource $couchbase)
 */
PHP_FUNCTION(couchbase_flush) {
    php_couchbase_flush_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto couchbase_get_result_code(resource $couchbase)
 */
PHP_FUNCTION(couchbase_get_result_code) {
    zval *res;
    php_couchbase_res *couchbase_res;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &res) == FAILURE) {
        return;
    }
    ZEND_FETCH_RESOURCE(couchbase_res, php_couchbase_res *, &res, -1, PHP_COUCHBASE_RESOURCE, le_couchbase);
    RETURN_LONG(couchbase_res->rc);
}
/* }}} */

/* {{{ proto couchbase_version(void)
 */
PHP_FUNCTION(couchbase_version) {
    RETURN_STRING(PHP_COUCHBASE_VERSION, 1);
}
/* }}} */

/* {{{ PHP_GINIT_FUNCTION
*/
PHP_GINIT_FUNCTION(couchbase) {
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(couchbase) {
    REGISTER_INI_ENTRIES();

    REGISTER_LONG_CONSTANT("COUCHBASE_SUCCESS",         LIBCOUCHBASE_SUCCESS, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("COUCHBASE_AUTH_CONTINUE",     LIBCOUCHBASE_AUTH_CONTINUE, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("COUCHBASE_AUTH_ERROR",         LIBCOUCHBASE_AUTH_ERROR, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("COUCHBASE_DELTA_BADVAL",     LIBCOUCHBASE_DELTA_BADVAL, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("COUCHBASE_E2BIG",             LIBCOUCHBASE_E2BIG, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("COUCHBASE_EBUSY",             LIBCOUCHBASE_EBUSY, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("COUCHBASE_EINTERNAL",         LIBCOUCHBASE_EINTERNAL, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("COUCHBASE_EINVAL",             LIBCOUCHBASE_EINVAL, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("COUCHBASE_ENOMEM",             LIBCOUCHBASE_ENOMEM, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("COUCHBASE_ERANGE",             LIBCOUCHBASE_ERANGE, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("COUCHBASE_ERROR",             LIBCOUCHBASE_ERROR, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("COUCHBASE_ETMPFAIL",         LIBCOUCHBASE_ETMPFAIL, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("COUCHBASE_KEY_EEXISTS",     LIBCOUCHBASE_KEY_EEXISTS, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("COUCHBASE_KEY_ENOENT",         LIBCOUCHBASE_KEY_ENOENT, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("COUCHBASE_NETWORK_ERROR",     LIBCOUCHBASE_NETWORK_ERROR, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("COUCHBASE_NOT_MY_VBUCKET",     LIBCOUCHBASE_NOT_MY_VBUCKET, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("COUCHBASE_NOT_STORED",         LIBCOUCHBASE_NOT_STORED, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("COUCHBASE_NOT_SUPPORTED",     LIBCOUCHBASE_NOT_SUPPORTED, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("COUCHBASE_UNKNOWN_COMMAND", LIBCOUCHBASE_UNKNOWN_COMMAND, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("COUCHBASE_UNKNOWN_HOST",     LIBCOUCHBASE_UNKNOWN_HOST, CONST_PERSISTENT | CONST_CS);

    le_couchbase = zend_register_list_destructors_ex(php_couchbase_res_dtor, NULL, PHP_COUCHBASE_RESOURCE, module_number);

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(couchbase) {
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(couchbase) {
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(couchbase) {
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
*/
PHP_MINFO_FUNCTION(couchbase)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "couchbase support", "enabled");
    php_info_print_table_end();

    DISPLAY_INI_ENTRIES();
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet expandtab sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
