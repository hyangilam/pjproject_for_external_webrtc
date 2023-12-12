/* $Id: pidf.h 3553 2011-05-05 06:14:19Z nanang $ */
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
#ifndef __PJSIP_SIMPLE_CONF_INFO_H__
#define __PJSIP_SIMPLE_CONF_INFO_H__

/**
 * @file conference_info.h
 * @brief CONFERENCE-INFO/Event Package for Conference State (RFC 4575)
 */
#include <pjsip-simple/types.h>
#include <pjlib-util/xml.h>


PJ_BEGIN_DECL

//////////////////////////////////////////////////////////////////
// + rfc 4575
//////////////////////////////////////////////////////////////////
typedef struct pjsip_conf_keywords_type pjsip_conf_keywords_type;

struct pjsip_conf_keywords_type {
	PJ_DECL_LIST_MEMBER(pjsip_conf_keywords_type);
	pj_str_t keyword;
};

typedef struct pjsip_conf_execution_type {
    #define         CONF_EXEC_ELEM_WHEN             1
    #define         CONF_EXEC_ELEM_REASON           1<<1
    #define         CONF_EXEC_ELEM_BY               1<<2
    pj_uint16_t     flag;

	pj_str_t 	name;		// "execution-type"

	// elements
	pj_str_t 	when;		// type="xs:dateTime" minOccurs="0"
	pj_str_t	reason;
	pj_str_t	by;			// type="xs:anyURI"
} pjsip_conf_execution_type;

typedef enum pjsip_conf_media_status_type {
	PJSIP_CONF_MEDIA_STATUS_RECVONLY,
	PJSIP_CONF_MEDIA_STATUS_SENDONLY,
	PJSIP_CONF_MEDIA_STATUS_SENDRECV,
	PJSIP_CONF_MEDIA_STATUS_INACTIVE
} pjsip_conf_media_status_type;


typedef struct pjsip_conf_medium_type {
    #define         CONF_MEDIUM_ELEM_DISPLAY_TEXT           1
    #define         CONF_MEDIUM_ELEM_TYPE                   1<<1
    #define         CONF_MEDIUM_ELEM_STATUS                 1<<2
    pj_uint16_t     flag;

	pj_str_t 	name;		// "conference-medium-type"

	// elements
	pj_str_t	display_text;
	pj_str_t	type;
	pjsip_conf_media_status_type	status;		// minOccurs="0"

} pjsip_conf_medium_type;

typedef struct pjsip_conf_medium pjsip_conf_medium;
struct pjsip_conf_medium{
	PJ_DECL_LIST_MEMBER(pjsip_conf_medium);
	pjsip_conf_medium_type medium;
};

typedef struct pjsip_conf_conference_media_type {
    #define         CONF_MEDIA_ELEM_ENTRY                   1
    pj_uint16_t     flag;

	pj_str_t 	name;		// "conference-media-type"

	// elements
	pjsip_conf_medium	entry_list;	// maxOccurs="unbounded"

} pjsip_conf_conference_media_type;


typedef enum pjsip_conf_state_enum {
	PJSIP_CONF_STATE_FULL,
	PJSIP_CONF_STATE_PARTIAL,
	PJSIP_CONF_STATE_DELETED
} pjsip_conf_state_enum;    

typedef struct pjsip_conf_uri_type {
    #define         CONF_URI_ELEM_URI                       1
    #define         CONF_URI_ELEM_DISPLAY_TEXT              1<<1
    #define         CONF_URI_ELEM_PURPOSE                   1<<2
    #define         CONF_URI_ELEM_MODIFIED                  1<<3
    pj_uint16_t     flag;

	pj_str_t name;

	// elements
	pj_str_t uri;					// type="xs:anyURI"
	pj_str_t display_text;			// minOccurs="0"
	pj_str_t purpose;				// minOccurs="0"
	pjsip_conf_execution_type	modified;
	// attributes
} pjsip_conf_uri_type;

typedef struct pjsip_conf_uri pjsip_conf_uri;

struct pjsip_conf_uri {
	PJ_DECL_LIST_MEMBER(pjsip_conf_uri);
	pjsip_conf_uri_type uri;
};


typedef struct pjsip_conf_uris_type {
    #define         CONF_URIS_ELEM_ENTRY                    1
    #define         CONF_URIS_ATTR_STATE                    1<<1
    pj_uint16_t     flag;

	pj_str_t name;

	// elements
	pjsip_conf_uri entry_list;		// maxOccurs="unbounded"
	// attributes
	pjsip_conf_state_enum	state;	// use="optional" default="full"
} pjsip_conf_uris_type;

typedef struct pjsip_conf_host_type{
    #define         CONF_HOST_ELEM_DISPLAY_TEXT                 1
    #define         CONF_HOST_ELEM_WEB_PAGE                     1<<1
    #define         CONF_HOST_ELEM_URIS                         1<<2
    pj_uint16_t     flag;

	pj_str_t name;				// "host-type"

	// elements
	pj_str_t display_text;		// minOccurs="0"
	pj_str_t web_page;			// type="xs:anyURI"  minOccurs="0"
	pjsip_conf_uris_type uris;		// minOccurs="0"
} pjsip_conf_host_type;

typedef struct pjsip_conf_state_type {
    #define         CONF_STATE_ELEM_USER_COUNT                  1
    #define         CONF_STATE_ELEM_ACTIVE                      1<<1
    #define         CONF_STATE_ELEM_LOCKED                      1<<2
    pj_uint16_t     flag;

	pj_str_t name;				// "conference-state"

    pj_uint8_t user_count;
    pj_bool_t active;
    pj_bool_t locked;
}pjsip_conf_state_type;

typedef struct pjsip_conf_user_role pjsip_conf_user_role;
struct pjsip_conf_user_role{
	PJ_DECL_LIST_MEMBER(pjsip_conf_user_role);
	pj_str_t role;
};

typedef struct pjsip_conf_user_roles_type{
    #define         CONF_USER_ROLES_ELEM_ENTRY              1
    pj_uint16_t     flag;

	pj_str_t name;				// "user-roles-type"

	// elements
	pjsip_conf_user_role entry_list;	// maxOccurs="unbounded"
} pjsip_conf_user_roles_type;

typedef struct pjsip_conf_user_lang pjsip_conf_user_lang;
struct pjsip_conf_user_lang{
	PJ_DECL_LIST_MEMBER(pjsip_conf_user_lang);
	pj_str_t langCode;
};

typedef struct pjsip_conf_user_languages_type{
    #define         CONF_USER_LANG_ELEM_ENTRY              1
    pj_uint16_t     flag;

	pj_str_t name;				// "user-languages-type"

	pjsip_conf_user_lang lang_list;
} pjsip_conf_user_languages_type;

typedef enum pjsip_conf_joining_type{
	PJSIP_CONF_JOINING_TYPE_DIALED_IN,
	PJSIP_CONF_JOINING_TYPE_DIALED_OUT,
	PJSIP_CONF_JOINING_TYPE_FOCUS_OWNER
}pjsip_conf_joining_type;

typedef enum pjsip_conf_endpoint_status_type{
	PJSIP_CONF_ENDPOINT_STATUS_PENDING,
	PJSIP_CONF_ENDPOINT_STATUS_DIALING_OUT,
	PJSIP_CONF_ENDPOINT_STATUS_DIALING_IN,
	PJSIP_CONF_ENDPOINT_STATUS_ALERTING,
	PJSIP_CONF_ENDPOINT_STATUS_ON_HOLD,
	PJSIP_CONF_ENDPOINT_STATUS_CONNECTED,
	PJSIP_CONF_ENDPOINT_STATUS_MUTED_VIA_FOCUS,
	PJSIP_CONF_ENDPOINT_STATUS_DISCONNECTING,
	PJSIP_CONF_ENDPOINT_STATUS_DISCONNECTED
}pjsip_conf_endpoint_status_type;

typedef enum pjsip_conf_disconnection_type{
	PJSIP_CONF_DISCONNECTION_TYPE_DEPARTED,
	PJSIP_CONF_DISCONNECTION_TYPE_BOOTED,
	PJSIP_CONF_DISCONNECTION_TYPE_FAILED,
	PJSIP_CONF_DISCONNECTION_TYPE_BUSY
}pjsip_conf_disconnection_type;

typedef struct pjsip_conf_sip_dialog_id_type{
    #define         CONF_SIP_DLG_ELEM_DISPLAY_TEXT              1
    #define         CONF_SIP_DLG_ELEM_CALL_ID                   1<<1
    #define         CONF_SIP_DLG_ELEM_FROM_TAG                  1<<2
    #define         CONF_SIP_DLG_ELEM_TO_TAG                    1<<3
    pj_uint16_t     flag;

	pj_str_t name;				// "sip-dialog-id-type"

	// elements
	pj_str_t display_text;		// minOccurs="0"
	pj_str_t call_id;			
	pj_str_t from_tag;
	pj_str_t to_tag;
}pjsip_conf_sip_dialog_id_type;

typedef struct pjsip_conf_call_type{
    #define         CONF_CALL_ELEM_SIP               1
    pj_uint16_t     flag;

	pj_str_t name;				// "call-type"

	// choice element
	pjsip_conf_sip_dialog_id_type sip;	

}pjsip_conf_call_type;

typedef struct pjsip_conf_media_type {
    #define         CONF_MEDIA_ELEM_DISPLAY_TEXT            1
    #define         CONF_MEDIA_ELEM_TYPE                    1<<1
    #define         CONF_MEDIA_ELEM_LABEL                   1<<2
    #define         CONF_MEDIA_ELEM_SRC_ID                  1<<3
    #define         CONF_MEDIA_ELEM_STATUS                  1<<4
    #define         CONF_MEDIA_ATTR_ID                      1<<5
    pj_uint16_t     flag;

	pj_str_t name;			// "media-type"

	// elements
	pj_str_t display_text;	// minOccurs="0"
	pj_str_t type;	// minOccurs="0"
	pj_str_t label;	// minOccurs="0"
	pj_str_t src_id;	// minOccurs="0"
	pjsip_conf_media_status_type	status;	// minOccurs="0"

	// attribute
	pj_str_t id;		// use="required"

}pjsip_conf_media_type;

typedef struct pjsip_conf_media pjsip_conf_media;
struct pjsip_conf_media{
	PJ_DECL_LIST_MEMBER(pjsip_conf_media);
	pjsip_conf_media_type media;
};

// permissible for partial notifications
// Element <endpoint> uses as the key 'entity'
typedef struct pjsip_conf_endpoint_type{
    #define         CONF_ENDPO_ELEM_DISPLAY_TEXT                1
    #define         CONF_ENDPO_ELEM_REFERRED                    1<<1
    #define         CONF_ENDPO_ELEM_STATUS                      1<<2
    #define         CONF_ENDPO_ELEM_JOINING_METHOD              1<<3
    #define         CONF_ENDPO_ELEM_JOINING_INFO                1<<4
    #define         CONF_ENDPO_ELEM_DISCONN_METHOD              1<<5
    #define         CONF_ENDPO_ELEM_DISCONN_INFO                1<<6
    #define         CONF_ENDPO_ELEM_MEDIA                       1<<7
    #define         CONF_ENDPO_ELEM_CALL_INFO                   1<<8
    #define         CONF_ENDPO_ATTR_ENTITY                      1<<9
    #define         CONF_ENDPO_ATTR_STATE                       1<<10
    pj_uint16_t     flag;

	pj_str_t name;				// "endpoint-type"

	// elements
	pj_str_t display_text;		// minOccurs="0"
	pjsip_conf_execution_type referred;	// minOccurs="0"
	pjsip_conf_endpoint_status_type status;// minOccurs="0"
	pjsip_conf_joining_type joining_method;// minOccurs="0"
	pjsip_conf_execution_type joining_info;// minOccurs="0"
	pjsip_conf_disconnection_type disconnection_method;	// minOccurs="0"
	pjsip_conf_execution_type disconnection_info;	// minOccurs="0"
	pjsip_conf_media media_list;	// minOccurs="0" maxOccurs="unbounded"
	pjsip_conf_call_type call_info;	// minOccurs="0"

	// attributes
	pj_str_t entity;				// key value
	pjsip_conf_state_enum state;	// use="optional" default="full"
}pjsip_conf_endpoint_type;


typedef struct pjsip_conf_endpoint pjsip_conf_endpoint;
struct pjsip_conf_endpoint{
	PJ_DECL_LIST_MEMBER(pjsip_conf_endpoint);
	pjsip_conf_endpoint_type endpoint;
};

// Element <user> uses as the key 'entity'
typedef struct pjsip_conf_user_type {
    #define         CONF_USER_ELEM_DISPLAY_TEXT                1
    #define         CONF_USER_ELEM_ASSOC_AORS                  1<<1
    #define         CONF_USER_ELEM_ROLES                       1<<2
    #define         CONF_USER_ELEM_LANGS                       1<<3
    #define         CONF_USER_ELEM_CASCADED_FOCUS              1<<4
    #define         CONF_USER_ELEM_ENDPOINT                    1<<5
    #define         CONF_USER_ATTR_ENTITY                      1<<6
    #define         CONF_USER_ATTR_STATE                       1<<7
    pj_uint16_t     flag;
    
	pj_str_t name;				// "user-type"

	// elements
	pj_str_t display_text;		// minOccurs="0"
	pjsip_conf_uris_type associated_aors;		// minOccurs="0"
	pjsip_conf_user_roles_type roles;			// minOccurs="0"
	pjsip_conf_user_languages_type languages; 	// minOccurs="0"
	pj_str_t cascaded_focus;	// type="xs:anyURI" minOccurs="0"
	pjsip_conf_endpoint endpoint_list;		// minOccurs="0" maxOccurs="unbounded"	// permissible for partial notifications

	// attributes
	pj_str_t entity;			// type="xs:anyURI"		// key value
	pjsip_conf_state_enum state;	// use="optional" default="full"
} pjsip_conf_user_type;

typedef struct pjsip_conf_user pjsip_conf_user;
struct pjsip_conf_user{
	PJ_DECL_LIST_MEMBER(pjsip_conf_user);
	pjsip_conf_user_type user;
};

typedef struct pjsip_conf_users_type {
    #define         CONF_USERS_ELEM_USER                1
    #define         CONF_USERS_ATTR_STATE               1<<1
    pj_uint16_t     flag;

	pj_str_t name;				// "users-type"

	// elements
	pjsip_conf_user *user_list;		// minOccurs="0" maxOccurs="unbounded" 	// permissible for partial notifications
	// attributes
	pjsip_conf_state_enum state;	// use="optional" default="full"
} pjsip_conf_users_type;

typedef struct pjsip_conf_description_type {
    #define         CONF_DESC_ELEM_DISPLAY_TEXT         1
    #define         CONF_DESC_ELEM_SUBJECT              1<<1
    #define         CONF_DESC_ELEM_FREE_TEXT            1<<2
    #define         CONF_DESC_ELEM_KEYWORDS             1<<3
    #define         CONF_DESC_ELEM_CONF_URIS            1<<4
    #define         CONF_DESC_ELEM_SERVICE_URIS         1<<5
    #define         CONF_DESC_ELEM_MAX_USER_COUNT       1<<6
    #define         CONF_DESC_ELEM_AVAIL_MEDIA          1<<7
    pj_uint16_t     flag;

	pj_str_t	name;		// "conference-description"

	// elements
	pj_str_t 	display_text;				// minOccurs="0"
	pj_str_t	subject;					// minOccurs="0"
	pj_str_t	free_text;					// minOccurs="0"
	pjsip_conf_keywords_type	keywords;		// minOccurs="0"
	pjsip_conf_uris_type	conf_uris;			// minOccurs="0"
	pjsip_conf_uris_type	service_uris;		// minOccurs="0"
	pj_uint8_t	maximum_user_count;			// minOccurs="0"
	pjsip_conf_conference_media_type	available_media;	// minOccurs="0"

} pjsip_conf_description_type;

typedef struct pjsip_conf_status_type {
    #define         CONF_DESC_ELEM_CONNECTED         	1
    #define         CONF_DESC_ELEM_TOTAL              	1<<1
    #define         CONF_DESC_ELEM_EVENT            	1<<2
    #define         CONF_DESC_ELEM_CONFERENCEID         1<<3
	#define         CONF_DESC_ELEM_CAUSEDBY	         	1<<4

    pj_uint16_t     flag;

	pj_str_t	name;		// "status"

	// elements
	pj_uint16_t 	connected;
	pj_uint16_t		total;
	pj_str_t		event;
	pj_str_t		conferenceId;
	pj_str_t		causedby;

} pjsip_conf_status_type;


typedef struct pjsip_conf_sidebars_by_val_type pjsip_conf_sidebars_by_val_type;

typedef struct pjsip_conf_type pjsip_conf_type;
// permissible for partial notifications
struct pjsip_conf_type {
	pj_bool_t		_is_valid;	/**< Internal flag.		    */

	#define         CONF_INFO_ELEM_STATUS             1
    pj_uint16_t     flag;

	pj_str_t name;				// "conference-info"
	pjsip_conf_status_type		status;
};

typedef struct pjsip_conf_type_list pjsip_conf_type_list;
struct pjsip_conf_type_list{
	PJ_DECL_LIST_MEMBER(pjsip_conf_type_list);
	pjsip_conf_type value;
};

struct pjsip_conf_sidebars_by_val_type{
	pj_str_t name;				// "sidebars-by-val-type"

	// elements
	pjsip_conf_type_list entry;	// minOccurs="0" maxOccurs="unbounded"

	// attributes
	pjsip_conf_state_enum state;	// use="optional" default="full"
};

extern pj_pool_t       *g_conf_info_pool;
extern pjsip_conf_type g_conf_info_var;
//////////////////////////////////////////////////////////////////
// - rfc 4575
//////////////////////////////////////////////////////////////////


/**
 * @defgroup PJSIP_SIMPLE_CONFERENCE_INFO CONFERENCE-INFO/Event Package for Conference State (RFC 4575)
 * @ingroup PJSIP_SIMPLE
 * @brief Support for CONFERENCE-INFO/Event Package for Conference State (RFC 4575)
 * @{
 *
 * This file provides tools for manipulating Event Package for Conference
 * State (CONFERENCE-INFO) as described in RFC 4575.
 */
typedef struct pj_xml_node pjconfinfo_xml_node_conf;
typedef struct pj_xml_node pjconfinfo_xml_node_tuple;
typedef struct pj_xml_node pjconfinfo_xml_node_status;
typedef struct pj_xml_node pjconfinfo_xml_node_note;

typedef struct pjconfinfo_status_op
{
    void	    (*construct)(pj_pool_t*, pjconfinfo_xml_node_status*);
    pj_bool_t	    (*is_basic_open)(const pjconfinfo_xml_node_status*);
    void	    (*set_basic_open)(pjconfinfo_xml_node_status*, pj_bool_t);
} pjconfinfo_status_op;

typedef struct pjconfinfo_tuple_op
{
    void	    (*construct)(pj_pool_t*, pjconfinfo_xml_node_tuple*, const pj_str_t*);

    const pj_str_t* (*get_id)(const pjconfinfo_xml_node_tuple* );
    void	    (*set_id)(pj_pool_t*, pjconfinfo_xml_node_tuple *, const pj_str_t*);

    pjconfinfo_xml_node_status*  (*get_status)(pjconfinfo_xml_node_tuple* );

    const pj_str_t* (*get_contact)(const pjconfinfo_xml_node_tuple*);
    void	    (*set_contact)(pj_pool_t*, pjconfinfo_xml_node_tuple*, const pj_str_t*);
    void	    (*set_contact_prio)(pj_pool_t*, pjconfinfo_xml_node_tuple*, const pj_str_t*);
    const pj_str_t* (*get_contact_prio)(const pjconfinfo_xml_node_tuple*);

    pjconfinfo_xml_node_note*    (*add_note)(pj_pool_t*, pjconfinfo_xml_node_tuple*, const pj_str_t*);
    pjconfinfo_xml_node_note*    (*get_first_note)(pjconfinfo_xml_node_tuple*);
    pjconfinfo_xml_node_note*    (*get_next_note)(pjconfinfo_xml_node_tuple*, pjconfinfo_xml_node_note*);

    const pj_str_t* (*get_timestamp)(const pjconfinfo_xml_node_tuple*);
    void	    (*set_timestamp)(pj_pool_t*, pjconfinfo_xml_node_tuple*, const pj_str_t*);
    void	    (*set_timestamp_np)(pj_pool_t*,pjconfinfo_xml_node_tuple*, pj_str_t*);

} pjconfinfo_tuple_op;

typedef struct pjconfinfo_conf_op
{
    void	    (*construct)(pj_pool_t*, pjconfinfo_xml_node_conf*, const pj_str_t*);

    pjconfinfo_xml_node_tuple*   (*add_tuple)(pj_pool_t*, pjconfinfo_xml_node_conf*, const pj_str_t*);
    pjconfinfo_xml_node_tuple*   (*get_first_tuple)(pjconfinfo_xml_node_conf*);
    pjconfinfo_xml_node_tuple*   (*get_next_tuple)(pjconfinfo_xml_node_conf*, pjconfinfo_xml_node_tuple*);
    pjconfinfo_xml_node_tuple*   (*find_tuple)(pjconfinfo_xml_node_conf*, const pj_str_t*);
    void	    (*remove_tuple)(pjconfinfo_xml_node_conf*, pjconfinfo_xml_node_tuple*);

    pjconfinfo_xml_node_note*    (*add_note)(pj_pool_t*, pjconfinfo_xml_node_conf*, const pj_str_t*);
    pjconfinfo_xml_node_note*    (*get_first_note)(pjconfinfo_xml_node_conf*);
    pjconfinfo_xml_node_note*    (*get_next_note)(pjconfinfo_xml_node_conf*, pjconfinfo_xml_node_note*);

} pjconfinfo_conf_op;


extern struct pjconfinfo_op_desc
{
    pjconfinfo_conf_op	pres;
    pjconfinfo_tuple_op	tuple;
    pjconfinfo_status_op	status;
} pjconfinfo_op;


/******************************************************************************
 * Top level API for managing conference document. 
 *****************************************************************************/
PJ_DECL(pjconfinfo_xml_node_conf*)    pjconfinfo_create(pj_pool_t *pool, const pj_str_t *entity);
PJ_DECL(pjconfinfo_xml_node_conf*)	 pjconfinfo_parse(pj_pool_t *pool, char *text, int len);
PJ_DECL(int)		 pjconfinfo_print(const pjconfinfo_xml_node_conf* pres, char *buf, int len);

/******************************************************************************
 * API for managing conference-info node.
 *****************************************************************************/
PJ_DECL(pj_status_t) pjconfinfo_parse_confinfo_elem(pj_pool_t *pool, pjconfinfo_xml_node_conf *confinfo_root_element, pjsip_conf_type *conf_info);

PJ_DECL(pj_status_t) pjconfinfo_parse_execution_type(pj_pool_t *pool, pj_xml_node *p_node, pjsip_conf_execution_type *p_type);
PJ_DECL(pj_status_t) pjconfinfo_parse_uri_type(pj_pool_t *pool, pj_xml_node *p_node, pjsip_conf_uri *p_type);
PJ_DECL(pj_status_t) pjconfinfo_parse_uris_type(pj_pool_t *pool, pj_xml_node *p_node, pjsip_conf_uris_type *p_type);
PJ_DECL(pj_status_t) pjconfinfo_parse_conf_desc_type(pj_pool_t *pool, pj_xml_node *conf_desc, pjsip_conf_description_type *conf_desc_type);

PJ_DECL(pj_status_t) pjconfinfo_parse_sip_dlg_id_type(pj_pool_t *pool, pj_xml_node *p_node, pjsip_conf_sip_dialog_id_type *p_type);
PJ_DECL(pj_status_t) pjconfinfo_parse_call_type(pj_pool_t *pool, pj_xml_node *p_node, pjsip_conf_call_type *p_type);
PJ_DECL(pj_status_t) pjconfinfo_parse_conf_state_type(pj_pool_t *pool, pj_xml_node *p_node, pjsip_conf_state_type *p_type);
PJ_DECL(pj_status_t) pjconfinfo_parse_media_type(pj_pool_t *pool, pj_xml_node *p_node, pjsip_conf_media *p_type);
PJ_DECL(pj_status_t) pjconfinfo_parse_endpoint_type(pj_pool_t *pool, pj_xml_node *p_node, pjsip_conf_endpoint *p_type);
PJ_DECL(pj_status_t) pjconfinfo_parse_user_type(pj_pool_t *pool, pj_xml_node *p_node, pjsip_conf_user *p_type);
PJ_DECL(pj_status_t) pjconfinfo_parse_users_type(pj_pool_t *pool, pj_xml_node *p_node, pjsip_conf_users_type *p_type);

PJ_DECL(pj_status_t) pjconfinfo_parse_status_type(pj_pool_t *pool, pj_xml_node *p_node, pjsip_conf_status_type *conf_status_type);

/******************************************************************************
 * API for managing Presence node.
 *****************************************************************************/
PJ_DECL(void)		 pjconfinfo_construct(pj_pool_t *pool, pjconfinfo_xml_node_conf *pres,
					       const pj_str_t *entity);
PJ_DECL(pjconfinfo_xml_node_tuple*)	 pjconfinfo_add_tuple(pj_pool_t *pool, pjconfinfo_xml_node_conf *pres, 
					       const pj_str_t *id);
PJ_DECL(pjconfinfo_xml_node_tuple*)	 pjconfinfo_get_first_tuple(pjconfinfo_xml_node_conf *pres);
PJ_DECL(pjconfinfo_xml_node_tuple*)	 pjconfinfo_get_next_tuple(pjconfinfo_xml_node_conf *pres, 
						    pjconfinfo_xml_node_tuple *t);
PJ_DECL(pjconfinfo_xml_node_tuple*)	 pjconfinfo_find_tuple(pjconfinfo_xml_node_conf *pres, 
						const pj_str_t *id);
PJ_DECL(void)		 pjconfinfo_remove_tuple(pjconfinfo_xml_node_conf *pres, 
						  pjconfinfo_xml_node_tuple*);

PJ_DECL(pjconfinfo_xml_node_note*)	 pjconfinfo_add_note(pj_pool_t *pool, pjconfinfo_xml_node_conf *pres, 
					      const pj_str_t *text);
PJ_DECL(pjconfinfo_xml_node_note*)	 pjconfinfo_get_first_note(pjconfinfo_xml_node_conf *pres);
PJ_DECL(pjconfinfo_xml_node_note*)	 pjconfinfo_get_next_note(pjconfinfo_xml_node_conf*, pjconfinfo_xml_node_note*);


/******************************************************************************
 * API for managing Tuple node.
 *****************************************************************************/
PJ_DECL(void)		 pjconfinfo_tuple_construct(pj_pool_t *pool, pjconfinfo_xml_node_tuple *t,
						const pj_str_t *id);
PJ_DECL(const pj_str_t*) pjconfinfo_tuple_get_id(const pjconfinfo_xml_node_tuple *t );
PJ_DECL(void)		 pjconfinfo_tuple_set_id(pj_pool_t *pool, pjconfinfo_xml_node_tuple *t, 
					     const pj_str_t *id);

PJ_DECL(pjconfinfo_xml_node_status*)  pjconfinfo_tuple_get_status(pjconfinfo_xml_node_tuple *t);

PJ_DECL(const pj_str_t*) pjconfinfo_tuple_get_contact(const pjconfinfo_xml_node_tuple *t);
PJ_DECL(void)		 pjconfinfo_tuple_set_contact(pj_pool_t *pool, pjconfinfo_xml_node_tuple *t, 
						  const pj_str_t *contact);
PJ_DECL(void)		 pjconfinfo_tuple_set_contact_prio(pj_pool_t *pool, pjconfinfo_xml_node_tuple *t, 
						       const pj_str_t *prio);
PJ_DECL(const pj_str_t*) pjconfinfo_tuple_get_contact_prio(const pjconfinfo_xml_node_tuple *t);

PJ_DECL(pjconfinfo_xml_node_note*)	 pjconfinfo_tuple_add_note(pj_pool_t *pool, pjconfinfo_xml_node_tuple *t,
					       const pj_str_t *text);
PJ_DECL(pjconfinfo_xml_node_note*)	 pjconfinfo_tuple_get_first_note(pjconfinfo_xml_node_tuple *t);
PJ_DECL(pjconfinfo_xml_node_note*)	 pjconfinfo_tuple_get_next_note(pjconfinfo_xml_node_tuple *t, pjconfinfo_xml_node_note *n);

PJ_DECL(const pj_str_t*) pjconfinfo_tuple_get_timestamp(const pjconfinfo_xml_node_tuple *t);
PJ_DECL(void)		 pjconfinfo_tuple_set_timestamp(pj_pool_t *pool, pjconfinfo_xml_node_tuple *t,
						    const pj_str_t *ts);
PJ_DECL(void)		 pjconfinfo_tuple_set_timestamp_np(	pj_pool_t*, pjconfinfo_xml_node_tuple *t,
							pj_str_t *ts);


/******************************************************************************
 * API for managing Status node.
 *****************************************************************************/
PJ_DECL(void)		 pjconfinfo_status_construct(pj_pool_t*, pjconfinfo_xml_node_status*);
PJ_DECL(pj_bool_t)	 pjconfinfo_status_is_basic_open(const pjconfinfo_xml_node_status*);
PJ_DECL(void)		 pjconfinfo_status_set_basic_open(pjconfinfo_xml_node_status*, pj_bool_t);


/**
 * @}
 */


PJ_END_DECL


#endif	/* __PJSIP_SIMPLE_CONF_INFO_H__ */
