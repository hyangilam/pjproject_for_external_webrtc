/* $Id: conference_info.c 3841 2011-10-24 09:28:13Z ming $ */
/* 
 * Copyright (C) 2008-2011 Teluu Inc. (http://www.teluu.com)
 * Copyright (C) 2003-2008 Benny Prijono <benny@prijono.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */
#include <pjsip-simple/errno.h>
#include <pjsip-simple/conference_info.h>
#include <pj/string.h>
#include <pj/pool.h>
#include <pj/assert.h>
#include <pj/log.h>

#define THIS_FILE   "conference_info"

struct pjconfinfo_op_desc pjconfinfo_op = 
{
    {
	&pjconfinfo_construct,
	&pjconfinfo_add_tuple,
	&pjconfinfo_get_first_tuple,
	&pjconfinfo_get_next_tuple,
	&pjconfinfo_find_tuple,
	&pjconfinfo_remove_tuple,
	&pjconfinfo_add_note,
	&pjconfinfo_get_first_note,
	&pjconfinfo_get_next_note
    },
    {
	&pjconfinfo_tuple_construct,
	&pjconfinfo_tuple_get_id,
	&pjconfinfo_tuple_set_id,
	&pjconfinfo_tuple_get_status,
	&pjconfinfo_tuple_get_contact,
	&pjconfinfo_tuple_set_contact,
	&pjconfinfo_tuple_set_contact_prio,
	&pjconfinfo_tuple_get_contact_prio,
	&pjconfinfo_tuple_add_note,
	&pjconfinfo_tuple_get_first_note,
	&pjconfinfo_tuple_get_next_note,
	&pjconfinfo_tuple_get_timestamp,
	&pjconfinfo_tuple_set_timestamp,
	&pjconfinfo_tuple_set_timestamp_np
    },
    {
	&pjconfinfo_status_construct,
	&pjconfinfo_status_is_basic_open,
	&pjconfinfo_status_set_basic_open
    }
};

// static pj_str_t PRESENCE = { "presence", 8 };
static pj_str_t CONFERENCE_INFO = { "conference-info", 15 };
static pj_str_t VERSION = {"version", 7 };


static pj_str_t ENTITY = { "entity", 6};
static pj_str_t	TUPLE = { "tuple", 5 };
static pj_str_t ID = { "id", 2 };
static pj_str_t NOTE = { "note", 4 };
static pj_str_t STATUS = { "status", 6 };
static pj_str_t CONTACT = { "contact", 7 };
static pj_str_t PRIORITY = { "priority", 8 };
static pj_str_t TIMESTAMP = { "timestamp", 9 };
static pj_str_t BASIC = { "basic", 5 };
static pj_str_t OPEN = { "open", 4 };
static pj_str_t CLOSED = { "closed", 6 };
static pj_str_t EMPTY_STRING = { NULL, 0 };

static pj_str_t XMLNS = { "xmlns", 5 };
static pj_str_t CONFINFO_XMLNS = { "urn:ietf:params:xml:ns:conference-info", 38 };

pj_pool_t *g_conf_info_pool = NULL;
pjsip_conf_type g_conf_info_var = {};


PJ_DEF(pjsip_conf_type*) pjsip_get_global_conf_info(void)
{
    return &g_conf_info_var;
}

static void xml_init_node(pj_pool_t *pool, pj_xml_node *node,
			  pj_str_t *name, const pj_str_t *value)
{
    pj_list_init(&node->attr_head);
    pj_list_init(&node->node_head);
    node->name = *name;
    if (value) pj_strdup(pool, &node->content, value);
    else node->content.ptr=NULL, node->content.slen=0;
}

static pj_xml_attr* xml_create_attr(pj_pool_t *pool, pj_str_t *name,
				    const pj_str_t *value)
{
    pj_xml_attr *attr = PJ_POOL_ALLOC_T(pool, pj_xml_attr);
    attr->name = *name;
    pj_strdup(pool, &attr->value, value);
    return attr;
}

/* Conference */
PJ_DEF(void) pjconfinfo_construct(pj_pool_t *pool, pjconfinfo_xml_node_conf *conf,
				   const pj_str_t *entity)
{
    pj_xml_attr *attr;

    xml_init_node(pool, conf, &CONFERENCE_INFO, NULL);
    attr = xml_create_attr(pool, &ENTITY, entity);
    pj_xml_add_attr(conf, attr);
    attr = xml_create_attr(pool, &XMLNS, &CONFINFO_XMLNS);
    pj_xml_add_attr(conf, attr);
}

PJ_DEF(pjconfinfo_xml_node_tuple*) pjconfinfo_add_tuple(pj_pool_t *pool, pjconfinfo_xml_node_conf *conf,
					    const pj_str_t *id)
{
    pjconfinfo_xml_node_tuple *t = PJ_POOL_ALLOC_T(pool, pjconfinfo_xml_node_tuple);
    pjconfinfo_tuple_construct(pool, t, id);
    pj_xml_add_node(conf, t);
    return t;
}

PJ_DEF(pjconfinfo_xml_node_tuple*) pjconfinfo_get_first_tuple(pjconfinfo_xml_node_conf *conf)
{
    return pj_xml_find_node(conf, &TUPLE);
}

PJ_DEF(pjconfinfo_xml_node_tuple*) pjconfinfo_get_next_tuple(pjconfinfo_xml_node_conf *conf, 
						 pjconfinfo_xml_node_tuple *tuple)
{
    return pj_xml_find_next_node(conf, tuple, &TUPLE);
}

static pj_bool_t find_tuple_by_id(const pj_xml_node *node, const void *id)
{
    return pj_xml_find_attr(node, &ID, (const pj_str_t*)id) != NULL;
}

PJ_DEF(pjconfinfo_xml_node_tuple*) pjconfinfo_find_tuple(pjconfinfo_xml_node_conf *conf, const pj_str_t *id)
{
    return pj_xml_find(conf, &TUPLE, id, &find_tuple_by_id);
}

PJ_DEF(void) pjconfinfo_remove_tuple(pjconfinfo_xml_node_conf *conf, pjconfinfo_xml_node_tuple *t)
{
    PJ_UNUSED_ARG(conf);
    pj_list_erase(t);
}

PJ_DEF(pjconfinfo_xml_node_note*) pjconfinfo_add_note(pj_pool_t *pool, pjconfinfo_xml_node_conf *conf, 
					  const pj_str_t *text)
{
    pjconfinfo_xml_node_note *note = PJ_POOL_ALLOC_T(pool, pjconfinfo_xml_node_note);
    xml_init_node(pool, note, &NOTE, text);
    pj_xml_add_node(conf, note);
    return note;
}

PJ_DEF(pjconfinfo_xml_node_note*) pjconfinfo_get_first_note(pjconfinfo_xml_node_conf *conf)
{
    return pj_xml_find_node( conf, &NOTE);
}

PJ_DEF(pjconfinfo_xml_node_note*) pjconfinfo_get_next_note(pjconfinfo_xml_node_conf *t, pjconfinfo_xml_node_note *note)
{
    return pj_xml_find_next_node(t, note, &NOTE);
}


/* Tuple */
PJ_DEF(void) pjconfinfo_tuple_construct(pj_pool_t *pool, pjconfinfo_xml_node_tuple *t,
				    const pj_str_t *id)
{
    pj_xml_attr *attr;
    pjconfinfo_xml_node_status *st;

    xml_init_node(pool, t, &TUPLE, NULL);
    attr = xml_create_attr(pool, &ID, id);
    pj_xml_add_attr(t, attr);
    st = PJ_POOL_ALLOC_T(pool, pjconfinfo_xml_node_status);
    pjconfinfo_status_construct(pool, st);
    pj_xml_add_node(t, st);
}

PJ_DEF(const pj_str_t*) pjconfinfo_tuple_get_id(const pjconfinfo_xml_node_tuple *t)
{
    const pj_xml_attr *attr = pj_xml_find_attr((pj_xml_node*)t, &ID, NULL);
    pj_assert(attr);
    return &attr->value;
}

PJ_DEF(void) pjconfinfo_tuple_set_id(pj_pool_t *pool, pjconfinfo_xml_node_tuple *t, const pj_str_t *id)
{
    pj_xml_attr *attr = pj_xml_find_attr(t, &ID, NULL);
    pj_assert(attr);
    pj_strdup(pool, &attr->value, id);
}


PJ_DEF(pjconfinfo_xml_node_status*) pjconfinfo_tuple_get_status(pjconfinfo_xml_node_tuple *t)
{
    pjconfinfo_xml_node_status *st = (pjconfinfo_xml_node_status*)pj_xml_find_node(t, &STATUS);
    pj_assert(st);
    return st;
}


PJ_DEF(const pj_str_t*) pjconfinfo_tuple_get_contact(const pjconfinfo_xml_node_tuple *t)
{
    pj_xml_node *node = pj_xml_find_node((pj_xml_node*)t, &CONTACT);
    if (!node)
	return &EMPTY_STRING;
    return &node->content;
}

PJ_DEF(void) pjconfinfo_tuple_set_contact(pj_pool_t *pool, pjconfinfo_xml_node_tuple *t, 
				      const pj_str_t *contact)
{
    pj_xml_node *node = pj_xml_find_node(t, &CONTACT);
    if (!node) {
	node = PJ_POOL_ALLOC_T(pool, pj_xml_node);
	xml_init_node(pool, node, &CONTACT, contact);
	pj_xml_add_node(t, node);
    } else {
	pj_strdup(pool, &node->content, contact);
    }
}

PJ_DEF(void) pjconfinfo_tuple_set_contact_prio(pj_pool_t *pool, pjconfinfo_xml_node_tuple *t, 
					   const pj_str_t *prio)
{
    pj_xml_node *node = pj_xml_find_node(t, &CONTACT);
    pj_xml_attr *attr;

    if (!node) {
	node = PJ_POOL_ALLOC_T(pool, pj_xml_node);
	xml_init_node(pool, node, &CONTACT, NULL);
	pj_xml_add_node(t, node);
    }
    attr = pj_xml_find_attr(node, &PRIORITY, NULL);
    if (!attr) {
	attr = xml_create_attr(pool, &PRIORITY, prio);
	pj_xml_add_attr(node, attr);
    } else {
	pj_strdup(pool, &attr->value, prio);
    }
}

PJ_DEF(const pj_str_t*) pjconfinfo_tuple_get_contact_prio(const pjconfinfo_xml_node_tuple *t)
{
    pj_xml_node *node = pj_xml_find_node((pj_xml_node*)t, &CONTACT);
    pj_xml_attr *attr;

    if (!node)
	return &EMPTY_STRING;
    attr = pj_xml_find_attr(node, &PRIORITY, NULL);
    if (!attr)
	return &EMPTY_STRING;
    return &attr->value;
}


PJ_DEF(pjconfinfo_xml_node_note*) pjconfinfo_tuple_add_note(pj_pool_t *pool, pjconfinfo_xml_node_tuple *t,
					   const pj_str_t *text)
{
    pjconfinfo_xml_node_note *note = PJ_POOL_ALLOC_T(pool, pjconfinfo_xml_node_note);
    xml_init_node(pool, note, &NOTE, text);
    pj_xml_add_node(t, note);
    return note;
}

PJ_DEF(pjconfinfo_xml_node_note*) pjconfinfo_tuple_get_first_note(pjconfinfo_xml_node_tuple *t)
{
    return pj_xml_find_node(t, &NOTE);
}

PJ_DEF(pjconfinfo_xml_node_note*) pjconfinfo_tuple_get_next_note(pjconfinfo_xml_node_tuple *t, pjconfinfo_xml_node_note *n)
{
    return pj_xml_find_next_node(t, n, &NOTE);
}


PJ_DEF(const pj_str_t*) pjconfinfo_tuple_get_timestamp(const pjconfinfo_xml_node_tuple *t)
{
    pj_xml_node *node = pj_xml_find_node((pj_xml_node*)t, &TIMESTAMP);
    return node ? &node->content : &EMPTY_STRING;
}

PJ_DEF(void) pjconfinfo_tuple_set_timestamp(pj_pool_t *pool, pjconfinfo_xml_node_tuple *t,
					const pj_str_t *ts)
{
    pj_xml_node *node = pj_xml_find_node(t, &TIMESTAMP);
    if (!node) {
	node = PJ_POOL_ALLOC_T(pool, pj_xml_node);
	xml_init_node(pool, node, &TIMESTAMP, ts);
	pj_xml_add_node(t, node);
    } else {
	pj_strdup(pool, &node->content, ts);
    }
}


PJ_DEF(void) pjconfinfo_tuple_set_timestamp_np(pj_pool_t *pool, pjconfinfo_xml_node_tuple *t, 
					   pj_str_t *ts)
{
    pj_xml_node *node = pj_xml_find_node(t, &TIMESTAMP);
    if (!node) {
	node = PJ_POOL_ALLOC_T(pool, pj_xml_node);
	xml_init_node(pool, node, &TIMESTAMP, ts);
    } else {
	node->content = *ts;
    }
}


/* Status */
PJ_DEF(void) pjconfinfo_status_construct(pj_pool_t *pool, pjconfinfo_xml_node_status *st)
{
    pj_xml_node *node;

    xml_init_node(pool, st, &STATUS, NULL);
    node = PJ_POOL_ALLOC_T(pool, pj_xml_node);
    xml_init_node(pool, node, &BASIC, &CLOSED);
    pj_xml_add_node(st, node);
}

PJ_DEF(pj_bool_t) pjconfinfo_status_is_basic_open(const pjconfinfo_xml_node_status *st)
{
    pj_xml_node *node = pj_xml_find_node((pj_xml_node*)st, &BASIC);
    if (!node)
	return PJ_FALSE;
    return pj_stricmp(&node->content, &OPEN)==0;
}

PJ_DEF(void) pjconfinfo_status_set_basic_open(pjconfinfo_xml_node_status *st, pj_bool_t open)
{
    pj_xml_node *node = pj_xml_find_node(st, &BASIC);
    if (node)
	node->content = open ? OPEN : CLOSED;
}

PJ_DEF(pjconfinfo_xml_node_conf*) pjconfinfo_create(pj_pool_t *pool, const pj_str_t *entity)
{
    pjconfinfo_xml_node_conf *conf = PJ_POOL_ALLOC_T(pool, pjconfinfo_xml_node_conf);
    pjconfinfo_construct(pool, conf, entity);
    return conf;
}

PJ_DEF(pjconfinfo_xml_node_conf*) pjconfinfo_parse(pj_pool_t *pool, char *text, int len)
{
    pjconfinfo_xml_node_conf *conf = pj_xml_parse(pool, text, len);
    if (conf && conf->name.slen == 15) {
	    pj_str_t name;

        name.ptr = conf->name.ptr + (conf->name.slen - 15);
        name.slen = 15;

        if (pj_stricmp(&name, &CONFERENCE_INFO) == 0)
            return conf;
    }
    return NULL;
}

PJ_DEF(int) pjconfinfo_print(const pjconfinfo_xml_node_conf* conf, char *buf, int len)
{
    return pj_xml_print(conf, buf, len, PJ_TRUE);
}


/*
 * Some static constants.
 */
static const pj_str_t STR_VERSION	    = { "version", 7 };
static const pj_str_t STR_STATE	        = { "state", 5 };
static const pj_str_t STR_STATE_FULL	        = { "full", 4 };
static const pj_str_t STR_STATE_PARTIAL	        = { "partial", 7 };
static const pj_str_t STR_STATE_DELETED	        = { "deleted", 7 };

static const pj_str_t STR_XMLNS	        = { "xmlns", 5 };
static const pj_str_t STR_ENTITY	    = { "entity", 6 };
static const pj_str_t STR_ID	    = { "id", 2 };

static const pj_str_t STR_CONF_DESC = { "conference-description", 22 };
static const pj_str_t STR_HOST_INFO = { "host-info", 9 };
static const pj_str_t STR_CONF_STATE = { "conference-state", 16 };
static const pj_str_t STR_USERS = { "users", 5 };
static const pj_str_t STR_USER = { "user", 4 };
static const pj_str_t STR_MEDIA = { "media", 5 };
static const pj_str_t STR_CALL_INFO = { "call-info", 9 };
static const pj_str_t STR_SIDEBARS_BY_REF = { "sidebars-by-ref", 15 };
static const pj_str_t STR_ENTRY = { "entry", 5 };
static const pj_str_t STR_SIDEBARS_BY_VAL = { "sidebars-by-val", 15 };

static const pj_str_t STR_DISPLAY_TEXT = { "display-text", 12 };
static const pj_str_t STR_SUBJECT = { "subject", 7 };
static const pj_str_t STR_FREE_TEXT = { "free-text", 9 };
static const pj_str_t STR_KEYWORDS = { "keywords", 8 };
static const pj_str_t STR_CONF_URIS = { "conf-uris", 9 };
static const pj_str_t STR_SERVICE_URIS = { "service-uris", 9 };
static const pj_str_t STR_MAXIMUM_USER_COUNT = { "maximum-user-count", 18 };
static const pj_str_t STR_AVAILABLE_MEDIA = { "available-media", 15 };
static const pj_str_t STR_ASSOCIATED_AORS = { "associated-aors", 15 };
static const pj_str_t STR_ROLES = { "roles", 5 };
static const pj_str_t STR_LANGUAGES = { "languages", 9 };
static const pj_str_t STR_CASCADED_FOCUS = { "cascaded-focus", 14 };
static const pj_str_t STR_ENDPOINT = { "endpoint", 8 };
static const pj_str_t STR_REFERRED = { "referred", 8 };
static const pj_str_t STR_STATUS = { "status", 6 };
static const pj_str_t STR_JOINING_METHOD = { "joining-method", 14 };
static const pj_str_t STR_JOINING_INFO = { "joining-info", 12 };
static const pj_str_t STR_DISCONNECTION_METHOD = { "disconnection-method", 20 };
static const pj_str_t STR_DISCONNECTION_INFO = { "disconnection-info", 18 };

static const pj_str_t STR_URI = { "uri", 3 };
static const pj_str_t STR_PURPOSE = { "purpose", 7 };
static const pj_str_t STR_MODIFIED = { "modified", 8 };
static const pj_str_t STR_WHEN = { "when", 4 };
static const pj_str_t STR_REASON = { "reason", 6 };
static const pj_str_t STR_BY = { "by", 2 };
static const pj_str_t STR_USER_COUNT = { "user-count", 10 };
static const pj_str_t STR_ACTIVE = { "active", 6 };
static const pj_str_t STR_LOCKED = { "locked", 6 };
static const pj_str_t STR_TRUE = { "true", 4 };
static const pj_str_t STR_FALSE = { "false", 5 };

static const pj_str_t STR_PIDF_XML	    = { "pidf+xml", 8};
static const pj_str_t STR_XPIDF_XML	    = { "xpidf+xml", 9};
static const pj_str_t STR_APP_PIDF_XML	    = { "application/pidf+xml", 20 };
static const pj_str_t STR_APP_XPIDF_XML    = { "application/xpidf+xml", 21 };

static const pj_str_t STR_CONF3_STATUS	        = { "status", 6};
static const pj_str_t STR_CONF3_CONNECTED	    = { "connected", 9};
static const pj_str_t STR_CONF3_TOTAL	        = { "total", 5};
static const pj_str_t STR_CONF3_EVENT	        = { "event", 5};
static const pj_str_t STR_CONF3_CONFERENCEID	= { "conferenceId", 12};
static const pj_str_t STR_CONF3_CAUSEDBY        = { "causedby", 8};

PJ_DECL(pj_status_t) pjconfinfo_parse_execution_type(pj_pool_t *pool, pj_xml_node *p_node, pjsip_conf_execution_type *p_type)
{
    pj_status_t status = PJ_SUCCESS;
    
    pj_xml_node* c_node = p_node->node_head.next;
    
    while(c_node != &p_node->node_head){
        
        if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_WHEN)) {
            p_type->flag |= CONF_EXEC_ELEM_WHEN;
            pj_strdup(pool, &p_type->when, &c_node->content);
        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_REASON)) {
            p_type->flag |= CONF_EXEC_ELEM_REASON;
            pj_strdup(pool, &p_type->reason, &c_node->content);
        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_BY)) {
            p_type->flag |= CONF_EXEC_ELEM_BY;
            pj_strdup(pool, &p_type->by, &c_node->content);
        }
        
        c_node = c_node->next;
    }

out:
    return status;     
}

PJ_DECL(pj_status_t) pjconfinfo_parse_uri_type(pj_pool_t *pool, pj_xml_node *p_node, pjsip_conf_uri *p_type)
{
    pj_status_t status = PJ_SUCCESS;
    pj_xml_node* c_node = p_node->node_head.next;
    
    pj_strdup(pool,&p_type->uri.name, &p_node->name);
    while(c_node != &p_node->node_head){
        
        if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_URI)) {
            p_type->uri.flag |= CONF_URI_ELEM_URI;
            pj_strdup(pool, &p_type->uri.uri, &c_node->content);
        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_DISPLAY_TEXT)) {
            p_type->uri.flag |= CONF_URI_ELEM_DISPLAY_TEXT;
            pj_strdup(pool, &p_type->uri.display_text, &c_node->content);
        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_PURPOSE)) {
            p_type->uri.flag |= CONF_URI_ELEM_PURPOSE;
            pj_strdup(pool, &p_type->uri.purpose, &c_node->content);
        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_MODIFIED)) {
            p_type->uri.flag |= CONF_URI_ELEM_MODIFIED;

            pjconfinfo_parse_execution_type(pool, c_node, &p_type->uri.modified);
        }
        
        c_node = c_node->next;
    }

    if(!(p_type->uri.flag & CONF_URI_ELEM_URI)){
        status = PJSIP_SIMPLE_EBADCONFERENCEINFO;
        goto out;
    }
out:
    return status;    
}

PJ_DECL(pj_status_t) pjconfinfo_parse_uris_type(pj_pool_t *pool, pj_xml_node *p_node, pjsip_conf_uris_type *p_type)
{
    pj_status_t status = PJ_SUCCESS;
    pj_xml_node* c_node = p_node->node_head.next;

    pj_strdup(pool,&p_type->name, &p_node->name);
    
    // parse attribute
    pj_xml_attr* cur_attr = p_node->attr_head.next;
    while(cur_attr != &p_node->attr_head)
    {
        if(!pj_strcmp((const pj_str_t *)&cur_attr->name, &STR_STATE)) {
            p_type->flag |= CONF_URIS_ATTR_STATE;
            if( !pj_strcmp(&cur_attr->value, &STR_STATE_FULL)) {
                p_type->state = PJSIP_CONF_STATE_FULL;
            } else if( !pj_strcmp(&cur_attr->value, &STR_STATE_PARTIAL)) {
                p_type->state = PJSIP_CONF_STATE_PARTIAL;
            } else if( !pj_strcmp(&cur_attr->value, &STR_STATE_DELETED)) {
                p_type->state = PJSIP_CONF_STATE_DELETED;
            } else {
                status = PJSIP_SIMPLE_EBADCONFERENCEINFO;
                goto out;
            }
        }
        cur_attr = cur_attr->next;
    }
    if( !(p_type->flag & CONF_URIS_ATTR_STATE)) {
        p_type->flag |= CONF_URIS_ATTR_STATE;
        p_type->state = PJSIP_CONF_STATE_FULL;
    }

    // parse elements
    pj_list_init(&p_type->entry_list);
    while(c_node != &p_node->node_head){
        
        if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_ENTRY)) {
            p_type->flag |= CONF_URIS_ELEM_ENTRY;

            pjsip_conf_uri *uri = PJ_POOL_ZALLOC_T(pool, pjsip_conf_uri);
            status = pjconfinfo_parse_uri_type(pool, c_node, uri);
            // list head 는 정보를 담지 않는다.
            if(status == PJ_SUCCESS)
	            pj_list_push_back(&p_type->entry_list, uri);
        }
        
        c_node = c_node->next;
    }


out:
    return status;    
}

PJ_DECL(pj_status_t) pjconfinfo_parse_conf_desc_type(pj_pool_t *pool, pj_xml_node *p_node, pjsip_conf_description_type *p_type)
{
    pj_status_t status = PJ_SUCCESS;
    pj_xml_node* c_node = p_node->node_head.next;

    // pjsip_conf_description_type *desc_type = &conf_info->conf_description;
    pj_strdup(pool,&p_type->name, &p_node->name);

    while(c_node != &p_node->node_head){

        if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_DISPLAY_TEXT)) {
            p_type->flag |= CONF_DESC_ELEM_DISPLAY_TEXT;
            pj_strdup(pool,&p_type->display_text, &c_node->content);

        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_SUBJECT)) {
            p_type->flag |= CONF_DESC_ELEM_SUBJECT;
            pj_strdup(pool,&p_type->subject, &c_node->content);

        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_FREE_TEXT)) {
            p_type->flag |= CONF_DESC_ELEM_FREE_TEXT;
            pj_strdup(pool,&p_type->free_text, &c_node->content);

        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_KEYWORDS)) {
            p_type->flag |= CONF_DESC_ELEM_KEYWORDS;

            // TODO

        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_CONF_URIS)) {
            p_type->flag |= CONF_DESC_ELEM_CONF_URIS;

            status = pjconfinfo_parse_uris_type(pool, c_node, &p_type->conf_uris);
            if(status != PJ_SUCCESS)
                goto out;

        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_SERVICE_URIS)) {
            p_type->flag |= CONF_DESC_ELEM_SERVICE_URIS;

            status = pjconfinfo_parse_uris_type(pool, c_node, &p_type->service_uris);
            if(status != PJ_SUCCESS)
                goto out;

        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_MAXIMUM_USER_COUNT)) {
            p_type->flag |= CONF_DESC_ELEM_MAX_USER_COUNT;
            
            p_type->maximum_user_count = pj_strtoul(&c_node->content);

        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_AVAILABLE_MEDIA)) {
            p_type->flag |= CONF_DESC_ELEM_AVAIL_MEDIA;
            // TODO
        }
        c_node = c_node->next;
    }

out:
    return status;
}


PJ_DECL(pj_status_t) pjconfinfo_parse_conf_state_type(pj_pool_t *pool, pj_xml_node *p_node, pjsip_conf_state_type *p_type)
{
    pj_status_t status = PJ_SUCCESS;
    pj_xml_node* c_node = p_node->node_head.next;

    pj_strdup(pool,&p_type->name, &p_node->name);
    
    while(c_node != &p_node->node_head){
        
        if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_USER_COUNT)) {
            p_type->flag |= CONF_STATE_ELEM_USER_COUNT;
            p_type->user_count = pj_strtoul(&c_node->content);

        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_ACTIVE)) {
            p_type->flag |= CONF_STATE_ELEM_ACTIVE;

            if(!pj_strcmp(&c_node->content, &STR_TRUE)){
                p_type->active = PJ_TRUE;
            } else 
                p_type->active = PJ_FALSE;

        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_LOCKED)) {
            p_type->flag |= CONF_STATE_ELEM_LOCKED;

            if(!pj_strcmp(&c_node->content, &STR_TRUE)){
                p_type->locked = PJ_TRUE;
            } else 
                p_type->locked = PJ_FALSE;
        }
        
        c_node = c_node->next;
    }

out:
    return status;        
}

PJ_DECL(pj_status_t) pjconfinfo_parse_user_roles_type(pj_pool_t *pool, pj_xml_node *p_node, pjsip_conf_user_roles_type *p_type)
{
    pj_status_t status = PJ_SUCCESS;
    pj_xml_node* c_node = p_node->node_head.next;

    
    pj_strdup(pool,&p_type->name, &p_node->name);
    
    pj_list_init(&p_type->entry_list);

    while(c_node != &p_node->node_head){
        if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_ENTRY)) {
            p_type->flag |= CONF_USER_ROLES_ELEM_ENTRY;
            pjsip_conf_user_role *entry = PJ_POOL_ZALLOC_T(pool, pjsip_conf_user_role);
            pj_strdup(pool, &entry->role, &c_node->content);
            pj_list_push_back(&p_type->entry_list, entry);
        } 

        c_node = c_node->next;
    }

out:
    return status;  
}


PJ_DECL(pj_status_t) pjconfinfo_parse_user_languages_type(pj_pool_t *pool, pj_xml_node *p_node, pjsip_conf_user_languages_type *p_type)
{
    pj_status_t status = PJ_SUCCESS;
    pj_xml_node* c_node = p_node->node_head.next;

    pj_strdup(pool,&p_type->name, &p_node->name);
    
    pj_list_init(&p_type->lang_list);

    // How to parse xs:list ???
    // e.g.  en kr fr ???

    // while(c_node != &p_node->node_head){
    //     if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_ENTRY)) {
    //         p_type->flag |= CONF_USER_ROLES_ELEM_ENTRY;
    //         pjsip_conf_user_role *entry = PJ_POOL_ZALLOC_T(pool, pjsip_conf_user_role);
    //         pj_strdup(pool, &entry->role, &c_node->content);
    //         pj_list_push_back(&p_type->lang_list, entry);
    //     } 
        
    //     c_node = c_node->next;
    // }

out:
    return status;  
}

PJ_DECL(pj_status_t) pjconfinfo_parse_media_type(pj_pool_t *pool, pj_xml_node *p_node, pjsip_conf_media *p_type)
{
    pj_status_t status = PJ_SUCCESS;
    pj_xml_node* c_node = p_node->node_head.next;

    pj_strdup(pool,&p_type->media.name, &p_node->name);
    
    // parse attribute
    pj_xml_attr* cur_attr = p_node->attr_head.next;
    while(cur_attr != &p_node->attr_head)
    {
        if(!pj_strcmp((const pj_str_t *)&cur_attr->name, &STR_ID)) {
            p_type->media.flag |= CONF_MEDIA_ATTR_ID;
            pj_strdup(pool, &p_type->media.id, &cur_attr->value);
        }
        cur_attr = cur_attr->next;
    }
    if( !(p_type->media.flag & CONF_MEDIA_ATTR_ID)) {
        status = PJSIP_SIMPLE_EBADCONFERENCEINFO;
        goto out;
    }
    
    // parse elements
    while(c_node != &p_node->node_head){
        if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_DISPLAY_TEXT)) {
            p_type->media.flag |= CONF_MEDIA_ELEM_DISPLAY_TEXT;
            pj_strdup(pool, &p_type->media.display_text, &c_node->content);
        } else if(!pj_strcmp2((const pj_str_t *)&c_node->name, "type")) {
            p_type->media.flag |= CONF_MEDIA_ELEM_TYPE;
            pj_strdup(pool, &p_type->media.type, &c_node->content);
        } else if(!pj_strcmp2((const pj_str_t *)&c_node->name, "label")) {
            p_type->media.flag |= CONF_MEDIA_ELEM_LABEL;
            pj_strdup(pool, &p_type->media.label, &c_node->content);
        } else if(!pj_strcmp2((const pj_str_t *)&c_node->name, "src-id")) {
            p_type->media.flag |= CONF_MEDIA_ELEM_SRC_ID;
            pj_strdup(pool, &p_type->media.src_id, &c_node->content);
        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_STATUS)) {
            p_type->media.flag |= CONF_MEDIA_ELEM_STATUS;
            if(!pj_strcmp2(&c_node->content, "receonly")) {
                p_type->media.status = PJSIP_CONF_MEDIA_STATUS_RECVONLY;
            } else if(!pj_strcmp2(&c_node->content, "sendonly")) {
                p_type->media.status = PJSIP_CONF_MEDIA_STATUS_SENDONLY;
            } else if(!pj_strcmp2(&c_node->content, "sendrecv")) {
                p_type->media.status = PJSIP_CONF_MEDIA_STATUS_SENDRECV;
            } else if(!pj_strcmp2(&c_node->content, "inactive")) {
                p_type->media.status = PJSIP_CONF_MEDIA_STATUS_INACTIVE;
            }
        } 
        
        c_node = c_node->next;
    }

out:
    return status;  
}

PJ_DECL(pj_status_t) pjconfinfo_parse_sip_dlg_id_type(pj_pool_t *pool, pj_xml_node *p_node, pjsip_conf_sip_dialog_id_type *p_type)
{
    pj_status_t status = PJ_SUCCESS;
    pj_xml_node* c_node = p_node->node_head.next;

    pj_strdup(pool,&p_type->name, &p_node->name);
        
    // parse elements
    while(c_node != &p_node->node_head){
        if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_DISPLAY_TEXT)) {
           p_type->flag |= CONF_SIP_DLG_ELEM_DISPLAY_TEXT;
           pj_strdup(pool, &p_type->display_text, &c_node->content);
        } else if(!pj_strcmp2((const pj_str_t *)&c_node->name, "call-id")) {
           p_type->flag |= CONF_SIP_DLG_ELEM_CALL_ID;
           pj_strdup(pool, &p_type->call_id, &c_node->content);
        } else if(!pj_strcmp2((const pj_str_t *)&c_node->name, "from-tag")) {
           p_type->flag |= CONF_SIP_DLG_ELEM_FROM_TAG;
           pj_strdup(pool, &p_type->from_tag, &c_node->content);
        } else if(!pj_strcmp2((const pj_str_t *)&c_node->name, "to-tag")) {
           p_type->flag |= CONF_SIP_DLG_ELEM_TO_TAG;
           pj_strdup(pool, &p_type->to_tag, &c_node->content);
        } 
        
        c_node = c_node->next;
    }

    if( (p_type->flag & (CONF_SIP_DLG_ELEM_CALL_ID | CONF_SIP_DLG_ELEM_FROM_TAG | CONF_SIP_DLG_ELEM_TO_TAG)) != 
                                (CONF_SIP_DLG_ELEM_CALL_ID | CONF_SIP_DLG_ELEM_FROM_TAG | CONF_SIP_DLG_ELEM_TO_TAG)) {
        status = PJSIP_SIMPLE_EBADCONFERENCEINFO;
        goto out;
    }

out:
    return status;  
}


PJ_DECL(pj_status_t) pjconfinfo_parse_call_type(pj_pool_t *pool, pj_xml_node *p_node, pjsip_conf_call_type *p_type)
{
    pj_status_t status = PJ_SUCCESS;
    pj_xml_node* c_node = p_node->node_head.next;

    pj_strdup(pool,&p_type->name, &p_node->name);
        
    // parse elements
    while(c_node != &p_node->node_head){
        if(!pj_strcmp2((const pj_str_t *)&c_node->name, "sip")) {
            status = pjconfinfo_parse_sip_dlg_id_type(pool, c_node, &p_type->sip);
            if(status != PJ_SUCCESS)
                goto out;
            p_type->flag |= CONF_CALL_ELEM_SIP;
        } 
        
        c_node = c_node->next;
    }

out:
    return status;  
}

PJ_DECL(pj_status_t) pjconfinfo_parse_endpoint_type(pj_pool_t *pool, pj_xml_node *p_node, pjsip_conf_endpoint *p_type)
{
    pj_status_t status = PJ_SUCCESS;
    pj_xml_node* c_node = p_node->node_head.next;

    pj_strdup(pool,&p_type->endpoint.name, &p_node->name);

    // parse attribute
    pj_xml_attr* cur_attr = p_node->attr_head.next;
    while(cur_attr != &p_node->attr_head)
    {
        if(!pj_strcmp((const pj_str_t *)&cur_attr->name, &STR_ENTITY)) {
            p_type->endpoint.flag |= CONF_ENDPO_ATTR_ENTITY;
            pj_strdup(pool, &p_type->endpoint.entity, &cur_attr->value);
        }
        else if(!pj_strcmp((const pj_str_t *)&cur_attr->name, &STR_STATE)) {
            p_type->endpoint.flag |= CONF_ENDPO_ATTR_STATE;
            if( !pj_strcmp(&cur_attr->value, &STR_STATE_FULL)) {
                p_type->endpoint.state = PJSIP_CONF_STATE_FULL;
            } else if( !pj_strcmp(&cur_attr->value, &STR_STATE_PARTIAL)) {
                p_type->endpoint.state = PJSIP_CONF_STATE_PARTIAL;
            } else if( !pj_strcmp(&cur_attr->value, &STR_STATE_DELETED)) {
                p_type->endpoint.state = PJSIP_CONF_STATE_DELETED;
            } else {
                status = PJSIP_SIMPLE_EBADCONFERENCEINFO;
                goto out;
            }
        }
        cur_attr = cur_attr->next;
    }
    if( !(p_type->endpoint.flag & CONF_ENDPO_ATTR_ENTITY)) {
        status = PJSIP_SIMPLE_EBADCONFERENCEINFO;
        goto out;
    }
    if( !(p_type->endpoint.flag & CONF_ENDPO_ATTR_STATE)) {
        p_type->endpoint.flag |= CONF_ENDPO_ATTR_STATE;
        p_type->endpoint.state = PJSIP_CONF_STATE_FULL;
    }
    

    // parse elements
    pj_list_init(&p_type->endpoint.media_list);

    while(c_node != &p_node->node_head){
        if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_DISPLAY_TEXT)) {
            p_type->endpoint.flag |= CONF_ENDPO_ELEM_DISPLAY_TEXT;
            pj_strdup(pool, &p_type->endpoint.display_text, &c_node->content);

        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_REFERRED)) {
            status = pjconfinfo_parse_execution_type(pool, c_node, &p_type->endpoint.referred);
            if(status != PJ_SUCCESS)
                goto out;
            
            p_type->endpoint.flag |= CONF_ENDPO_ELEM_REFERRED;

        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_STATUS)) {
            if( !pj_strcmp2(&c_node->content, "pending")) {
                p_type->endpoint.status = PJSIP_CONF_ENDPOINT_STATUS_PENDING;
            } else if( !pj_strcmp2(&c_node->content, "dialing-out")) {
                p_type->endpoint.status = PJSIP_CONF_ENDPOINT_STATUS_DIALING_OUT;
            } else if( !pj_strcmp2(&c_node->content, "dialing-in")) {
                p_type->endpoint.status = PJSIP_CONF_ENDPOINT_STATUS_DIALING_IN;
            } else if( !pj_strcmp2(&c_node->content, "alerting")) {
                p_type->endpoint.status = PJSIP_CONF_ENDPOINT_STATUS_ALERTING;
            } else if( !pj_strcmp2(&c_node->content, "on-hold")) {
                p_type->endpoint.status = PJSIP_CONF_ENDPOINT_STATUS_ON_HOLD;
            } else if( !pj_strcmp2(&c_node->content, "connected")) {
                p_type->endpoint.status = PJSIP_CONF_ENDPOINT_STATUS_CONNECTED;
            } else if( !pj_strcmp2(&c_node->content, "muted-via-focus")) {
                p_type->endpoint.status = PJSIP_CONF_ENDPOINT_STATUS_MUTED_VIA_FOCUS;
            } else if( !pj_strcmp2(&c_node->content, "disconnecting")) {
                p_type->endpoint.status = PJSIP_CONF_ENDPOINT_STATUS_DISCONNECTING;
            } else if( !pj_strcmp2(&c_node->content, "disconnected")) {
                p_type->endpoint.status = PJSIP_CONF_ENDPOINT_STATUS_DISCONNECTED;
            } else {
                status = PJSIP_SIMPLE_EBADCONFERENCEINFO;
                goto out;
            }
            
            p_type->endpoint.flag |= CONF_ENDPO_ELEM_STATUS;

        }  else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_JOINING_METHOD)) {
            if( !pj_strcmp2(&c_node->content, "dialed-in")) {
                p_type->endpoint.joining_method = PJSIP_CONF_JOINING_TYPE_DIALED_IN;
            } else if( !pj_strcmp2(&c_node->content, "dialed-out")) {
                p_type->endpoint.joining_method = PJSIP_CONF_JOINING_TYPE_DIALED_OUT;
            } else if( !pj_strcmp2(&c_node->content, "focus-owner")) {
                p_type->endpoint.joining_method = PJSIP_CONF_JOINING_TYPE_FOCUS_OWNER;
            } else {
                status = PJSIP_SIMPLE_EBADCONFERENCEINFO;
                goto out;
            }
            
            p_type->endpoint.flag |= CONF_ENDPO_ELEM_JOINING_METHOD;

        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_JOINING_INFO)) {
            
            status = pjconfinfo_parse_execution_type(pool, c_node, &p_type->endpoint.joining_info);
            if(status != PJ_SUCCESS)
                goto out;
            p_type->endpoint.flag |= CONF_ENDPO_ELEM_JOINING_INFO;

        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_DISCONNECTION_METHOD)) {
            
             if( !pj_strcmp2(&c_node->content, "departed")) {
                p_type->endpoint.disconnection_method = PJSIP_CONF_DISCONNECTION_TYPE_DEPARTED;
            } else if( !pj_strcmp2(&c_node->content, "booted")) {
                p_type->endpoint.disconnection_method = PJSIP_CONF_DISCONNECTION_TYPE_BOOTED;
            } else if( !pj_strcmp2(&c_node->content, "failed")) {
                p_type->endpoint.disconnection_method = PJSIP_CONF_DISCONNECTION_TYPE_FAILED;
            } else if( !pj_strcmp2(&c_node->content, "busy")) {
                p_type->endpoint.disconnection_method = PJSIP_CONF_DISCONNECTION_TYPE_BUSY;
            } else {
                status = PJSIP_SIMPLE_EBADCONFERENCEINFO;
                goto out;
            }

            p_type->endpoint.flag |= CONF_ENDPO_ELEM_DISCONN_METHOD;

        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_DISCONNECTION_INFO)) {
            
            status = pjconfinfo_parse_execution_type(pool, c_node, &p_type->endpoint.disconnection_info);
            if(status != PJ_SUCCESS)
                goto out;
            p_type->endpoint.flag |= CONF_ENDPO_ELEM_DISCONN_INFO;

        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_MEDIA)) {
            pjsip_conf_media* media = PJ_POOL_ZALLOC_T(pool, pjsip_conf_media);

            status = pjconfinfo_parse_media_type(pool, c_node, media);
            if(status != PJ_SUCCESS)
                goto out;
            else
                pj_list_push_back(&p_type->endpoint.media_list, media);

            p_type->endpoint.flag |= CONF_ENDPO_ELEM_MEDIA;

        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_CALL_INFO)) {
            
            status = pjconfinfo_parse_call_type(pool, c_node, &p_type->endpoint.call_info);
            if(status != PJ_SUCCESS)
                goto out;
            p_type->endpoint.flag |= CONF_ENDPO_ELEM_CALL_INFO;

        } 
        
        c_node = c_node->next;
    }

out:
    return status;  
}
PJ_DECL(pj_status_t) pjconfinfo_parse_user_type(pj_pool_t *pool, pj_xml_node *p_node, pjsip_conf_user *p_type)
{
    pj_status_t status = PJ_SUCCESS;
    pj_xml_node* c_node = p_node->node_head.next;

    
    pj_strdup(pool,&p_type->user.name, &p_node->name);
    
    // parse attribute
    pj_xml_attr* cur_attr = p_node->attr_head.next;
    while(cur_attr != &p_node->attr_head)
    {
        if(!pj_strcmp((const pj_str_t *)&cur_attr->name, &STR_ENTITY)) {
            p_type->user.flag |= CONF_USER_ATTR_ENTITY;
            pj_strdup(pool, &p_type->user.entity, &cur_attr->value);
        }
        else if(!pj_strcmp((const pj_str_t *)&cur_attr->name, &STR_STATE)) {
            p_type->user.flag |= CONF_USER_ATTR_STATE;
            if( !pj_strcmp(&cur_attr->value, &STR_STATE_FULL)) {
                p_type->user.state = PJSIP_CONF_STATE_FULL;
            } else if( !pj_strcmp(&cur_attr->value, &STR_STATE_PARTIAL)) {
                p_type->user.state = PJSIP_CONF_STATE_PARTIAL;
            } else if( !pj_strcmp(&cur_attr->value, &STR_STATE_DELETED)) {
                p_type->user.state = PJSIP_CONF_STATE_DELETED;
            } else {
                status = PJSIP_SIMPLE_EBADCONFERENCEINFO;
                goto out;
            }
        }
        cur_attr = cur_attr->next;
    }
    if( !(p_type->user.flag & CONF_USER_ATTR_STATE)) {
        p_type->user.flag |= CONF_USER_ATTR_STATE;
        p_type->user.state = PJSIP_CONF_STATE_FULL;
    }


    // parse elements
    pj_list_init(&p_type->user.endpoint_list);

    while(c_node != &p_node->node_head){
        
        if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_DISPLAY_TEXT)) {
            p_type->user.flag |= CONF_USER_ELEM_DISPLAY_TEXT;

            pj_strdup(pool, &p_type->user.display_text, &c_node->content);

        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_ASSOCIATED_AORS)) {
            p_type->user.flag |= CONF_USER_ELEM_ASSOC_AORS;

            status = pjconfinfo_parse_uris_type(pool, c_node, &p_type->user.associated_aors);
            if(status != PJ_SUCCESS)
                goto out;

        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_ROLES)) {
            p_type->user.flag |= CONF_USER_ELEM_ROLES;

            status = pjconfinfo_parse_user_roles_type(pool, c_node, &p_type->user.roles);
            if(status != PJ_SUCCESS)
                goto out;

        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_LANGUAGES)) {
            p_type->user.flag |= CONF_USER_ELEM_LANGS;

            // TODO

            // status = pjconfinfo_parse_user_languages_type(pool, c_node, &p_type->user.languages);
            // if(status != PJ_SUCCESS)
            //     goto out;


        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_CASCADED_FOCUS)) {
            p_type->user.flag |= CONF_USER_ELEM_CASCADED_FOCUS;

            pj_strdup(pool, &p_type->user.cascaded_focus, &c_node->content);

        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_ENDPOINT)) {
            p_type->user.flag |= CONF_USER_ELEM_ENDPOINT;

            pjsip_conf_endpoint* endpoint = PJ_POOL_ZALLOC_T(pool, pjsip_conf_endpoint);
            status = pjconfinfo_parse_endpoint_type(pool, c_node, endpoint);
            if(status != PJ_SUCCESS)
                goto out;
            else
                pj_list_push_back(&p_type->user.endpoint_list, endpoint);

        }
        
        c_node = c_node->next;
    }

out:
    return status;  
}

PJ_DECL(pj_status_t) pjconfinfo_parse_users_type(pj_pool_t *pool, pj_xml_node *p_node, pjsip_conf_users_type *p_type)
{
    pj_status_t status = PJ_SUCCESS;
    pj_xml_node* c_node = p_node->node_head.next;

    pj_strdup(pool,&p_type->name, &p_node->name);

    // parse attribute
    pj_xml_attr* cur_attr = p_node->attr_head.next;
    while(cur_attr != &p_node->attr_head)
    {
        if(!pj_strcmp((const pj_str_t *)&cur_attr->name, &STR_STATE)) {
            p_type->flag |= CONF_USERS_ATTR_STATE;
            if( !pj_strcmp(&cur_attr->value, &STR_STATE_FULL)) {
                p_type->state = PJSIP_CONF_STATE_FULL;
            } else if( !pj_strcmp(&cur_attr->value, &STR_STATE_PARTIAL)) {
                p_type->state = PJSIP_CONF_STATE_PARTIAL;
            } else if( !pj_strcmp(&cur_attr->value, &STR_STATE_DELETED)) {
                p_type->state = PJSIP_CONF_STATE_DELETED;
            } else {
                status = PJSIP_SIMPLE_EBADCONFERENCEINFO;
                goto out;
            }
        }
        cur_attr = cur_attr->next;
    }
    if( !(p_type->flag & CONF_USERS_ATTR_STATE)) {
        p_type->flag |= CONF_USERS_ATTR_STATE;
        p_type->state = PJSIP_CONF_STATE_FULL;
    }

    // parse elements
    p_type->user_list = PJ_POOL_ZALLOC_T(pool, pjsip_conf_user);
    pj_list_init(p_type->user_list);
    while(c_node != &p_node->node_head){
        
        if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_USER)) {
            p_type->flag |= CONF_USERS_ELEM_USER;

            pjsip_conf_user *user = PJ_POOL_ZALLOC_T(pool, pjsip_conf_user);
            status = pjconfinfo_parse_user_type(pool, c_node, user);
            // list head 는 정보를 담지 않는다.
            if(status == PJ_SUCCESS)
	            pj_list_push_back(p_type->user_list, user);

        } 
        
        c_node = c_node->next;
    }

out:
    return status;        
}

PJ_DECL(pj_status_t) pjconfinfo_parse_status_type(pj_pool_t *pool, pj_xml_node *p_node, pjsip_conf_status_type *p_type)
{
    pj_status_t status = PJ_SUCCESS;
    pj_xml_node* c_node = p_node->node_head.next;

    pj_strdup(pool,&p_type->name, &p_node->name);

    while(c_node != &p_node->node_head){

        if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_CONF3_CONNECTED)) {
            p_type->flag |= CONF_DESC_ELEM_CONNECTED;
            p_type->connected = pj_strtoul(&c_node->content);

        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_CONF3_TOTAL)) {
            p_type->flag |= CONF_DESC_ELEM_TOTAL;
            p_type->total = pj_strtoul(&c_node->content);

        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_CONF3_EVENT)) {
            p_type->flag |= CONF_DESC_ELEM_EVENT;
            pj_strdup(pool,&p_type->event, &c_node->content);

        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_CONF3_CONFERENCEID)) {
            p_type->flag |= CONF_DESC_ELEM_CONFERENCEID;
            pj_strdup(pool,&p_type->conferenceId, &c_node->content);

        } else if(!pj_strcmp((const pj_str_t *)&c_node->name, &STR_CONF3_CAUSEDBY)) {
            p_type->flag |= CONF_DESC_ELEM_CAUSEDBY;
            pj_strdup(pool,&p_type->causedby, &c_node->content);

        }

        c_node = c_node->next;
    }

    if(!(p_type->flag & (CONF_DESC_ELEM_CONNECTED|CONF_DESC_ELEM_TOTAL))){
        status = PJSIP_SIMPLE_EBADCONFERENCEINFO;
        goto out;
    }

out:
    return status;
}


PJ_DECL(pj_status_t) pjconfinfo_parse_confinfo_elem(pj_pool_t *pool, pjconfinfo_xml_node_conf *confinfo_root_element, pjsip_conf_type *conf_info)
{
    pj_status_t status = PJ_SUCCESS;

    pj_xml_node* cur_sub_elem = confinfo_root_element->node_head.next;

    while(cur_sub_elem != &confinfo_root_element->node_head){

        if(!pj_strcmp((const pj_str_t *)&cur_sub_elem->name, &STR_CONF3_STATUS)) {
            conf_info->flag |= CONF_INFO_ELEM_STATUS;

            status = pjconfinfo_parse_status_type( pool, cur_sub_elem, &conf_info->status);
            if(status != PJ_SUCCESS)
                goto out;

        } 
        cur_sub_elem = cur_sub_elem->next;
    }

    if(!(conf_info->flag & (CONF_INFO_ELEM_STATUS))){
        status = PJSIP_SIMPLE_EBADCONFERENCEINFO;
        goto out;
    }
out:
    return status;
}
