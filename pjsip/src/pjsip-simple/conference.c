/* $Id: conference.c 4713 2014-01-23 08:13:11Z nanang $ */
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
#include <pjsip-simple/conference.h>
#include <pjsip-simple/errno.h>
#include <pjsip-simple/evsub_msg.h>
#include <pjsip/sip_module.h>
#include <pjsip/sip_multipart.h>
#include <pjsip/sip_endpoint.h>
#include <pjsip/sip_dialog.h>
#include <pjsip/sip_util.h>
#include <pjsua-lib/pjsua.h>
#include <pjsua-lib/pjsua_internal.h>
#include <pj/assert.h>
#include <pj/guid.h>
#include <pj/log.h>
#include <pj/os.h>
#include <pj/pool.h>
#include <pj/string.h>

// nugucall - conference

#define THIS_FILE		    "conference.c"
#define CONF_DEFAULT_EXPIRES	    PJSIP_CONF_DEFAULT_EXPIRES

#if PJSIP_CONF_BAD_CONTENT_RESPONSE < 200 || \
    PJSIP_CONF_BAD_CONTENT_RESPONSE > 699 || \
    PJSIP_CONF_BAD_CONTENT_RESPONSE/100 == 3
# error Invalid PJSIP_CONF_BAD_CONTENT_RESPONSE value
#endif

struct pjsua_data* gp_pjsua_var = NULL;

/* Notification on incoming request */
static pj_bool_t mod_conf_outdlg_noti_on_rx_request(pjsip_rx_data *rdata)
{
    pjsip_tx_data *tdata;
    pjsip_status_code status_code;
    pj_status_t status;

    // nugucall - conference-info simple outdialog
    if (rdata->msg_info.to->tag.slen == 0 &&
    rdata->msg_info.msg->line.req.method.id == PJSIP_OTHER_METHOD &&
        (pj_strncmp2(&rdata->msg_info.msg->line.req.method.name, "NOTIFY", 6) == 0))
    {
		// keep going
    } else {
		return PJ_FALSE;
	}

	pjsip_conf_type *conf_info;

	// conference index will be always 0
    // conf_info = &pjsua_var.conference[0].conf_info;
	conf_info = &gp_pjsua_var->conference[0].conf_info;
    
	status = pjsip_conf_parse_confinfo( rdata, g_conf_info_pool, conf_info);

	if(status != PJ_SUCCESS) {
		conf_info->_is_valid = PJ_FALSE;
		// pjsua_perror(THIS_FILE, "simple conference-info parse error", status);
        PJ_PERROR(5,(THIS_FILE, status, "simple conference-info parse erro"));
	} else {
		conf_info->_is_valid = PJ_TRUE;
	}
	
	// notify application when conference state changed
	// if (pjsua_var.ua_cfg.cb.on_conference_state)
	//     (*pjsua_var.ua_cfg.cb.on_conference_state)(0);
	if (gp_pjsua_var->ua_cfg.cb.on_conference_state)
	    (*gp_pjsua_var->ua_cfg.cb.on_conference_state)(0);

	// response 200 OK
	status_code = PJSIP_SC_OK;

    // status = pjsip_endpt_create_response(pjsua_get_pjsip_endpt(), 
	status = pjsip_endpt_create_response(gp_pjsua_var->endpt, 
					 rdata, status_code, 
					 NULL, &tdata);
    if (status != PJ_SUCCESS) {
	// pjsua_perror(THIS_FILE, "Unable to create response", status);
	PJ_PERROR(5,(THIS_FILE, status, "Unable to create response"));
	return PJ_TRUE;
    }

    // pjsip_endpt_send_response2(pjsua_get_pjsip_endpt(), rdata, tdata, 
	pjsip_endpt_send_response2(gp_pjsua_var->endpt, rdata, tdata, 
			       NULL, NULL);

    return PJ_TRUE;
}


/*
 * Conference module (mod-conference)
 */
static struct pjsip_module mod_conference = 
{
    NULL, NULL,			    /* prev, next.			*/
    { "mod-conference", 14 },	    /* Name.				*/
    -1,				    /* Id				*/
    PJSIP_MOD_PRIORITY_DIALOG_USAGE,/* Priority				*/
    NULL,			    /* load()				*/
    NULL,			    /* start()				*/
    NULL,			    /* stop()				*/
    NULL,			    /* unload()				*/
    &mod_conf_outdlg_noti_on_rx_request,			    /* on_rx_request()			*/ // nugucall - conference-info simple outdialog
	// NULL,				/* on_rx_request()			*/ // nugucall - conference-info simple outdialog
    NULL,			    /* on_rx_response()			*/
    NULL,			    /* on_tx_request.			*/
    NULL,			    /* on_tx_response()			*/
    NULL,			    /* on_tsx_state()			*/
};


/*
 * Conference message body type.
 */
typedef enum content_type_e
{
    CONTENT_TYPE_NONE,
    CONTENT_TYPE_CONFERENCE_INFO
    // CONTENT_TYPE_PIDF,
    // CONTENT_TYPE_XPIDF,
} content_type_e;

/*
 * This structure describe a confentity, for both subscriber and notifier.
 */
struct pjsip_conf
{
    pjsip_evsub		*sub;		/**< Event subscribtion record.	    */
    pjsip_dialog	*dlg;		/**< The dialog.		    */
    content_type_e	 content_type;	/**< Content-Type.		    */
    // pj_pool_t		*status_pool;	/**< Pool for conf_status	    */
    pj_pool_t       *conf_info_pool;
    // pjsip_conf_status	 status;	/**< Conference status.		    */ // SHOULD be removed
    pjsip_conf_type conf_info;  /**< Conference info        */

    pj_pool_t		*tmp_pool;	/**< Pool for tmp_conf_info	    */
    // pjsip_conf_status	 tmp_status;	/**< Temp, before NOTIFY is answred.*/  // SHOULD be removed
    pjsip_conf_type tmp_conf_info;  /**< Temp, before NOTIFY is answred.*/

    pjsip_evsub_user	 user_cb;	/**< The user callback.		    */
};


typedef struct pjsip_conf pjsip_conf;


/*
 * Forward decl for evsub callback.
 */
static void conf_on_evsub_state( pjsip_evsub *sub, pjsip_event *event);
static void conf_on_evsub_tsx_state( pjsip_evsub *sub, pjsip_transaction *tsx,
				     pjsip_event *event);
static void conf_on_evsub_rx_refresh( pjsip_evsub *sub, 
				      pjsip_rx_data *rdata,
				      int *p_st_code,
				      pj_str_t **p_st_text,
				      pjsip_hdr *res_hdr,
				      pjsip_msg_body **p_body);
static void conf_on_evsub_rx_notify( pjsip_evsub *sub, 
				     pjsip_rx_data *rdata,
				     int *p_st_code,
				     pj_str_t **p_st_text,
				     pjsip_hdr *res_hdr,
				     pjsip_msg_body **p_body);
static void conf_on_evsub_client_refresh(pjsip_evsub *sub);
static void conf_on_evsub_server_timeout(pjsip_evsub *sub);


/*
 * Event subscription callback for conference.
 */
static pjsip_evsub_user conf_user = 
{
    &conf_on_evsub_state,
    &conf_on_evsub_tsx_state,
    &conf_on_evsub_rx_refresh,
    &conf_on_evsub_rx_notify,
    &conf_on_evsub_client_refresh,
    &conf_on_evsub_server_timeout,
};


/*
 * Some static constants.
 */
static const pj_str_t STR_EVENT	    = { "Event", 5 };
static const pj_str_t STR_CONFERENCE	    = { "conference", 10 };
static const pj_str_t STR_APPLICATION	    = { "application", 11 };
static const pj_str_t STR_CONF_INFO_XML	    = { "conference-info+xml", 19};
// static const pj_str_t STR_PIDF_XML	    = { "pidf+xml", 8};
// static const pj_str_t STR_XPIDF_XML	    = { "xpidf+xml", 9};
static const pj_str_t STR_APP_CONF_INFO_XML	    = { "application/conference-info+xml", 31 };
// static const pj_str_t STR_APP_PIDF_XML	    = { "application/pidf+xml", 20 };
// static const pj_str_t STR_APP_XPIDF_XML    = { "application/xpidf+xml", 21 };


/*
 * Init conference module.
 */
PJ_DEF(pj_status_t) pjsip_conf_init_module( void* p_pjsua_var, pjsip_endpoint *endpt,
					    pjsip_module *mod_evsub)
{
    pj_status_t status;
    pj_str_t accept[1];

	gp_pjsua_var = (struct pjsua_data* )p_pjsua_var;

    /* Check arguments. */
    PJ_ASSERT_RETURN(endpt && mod_evsub, PJ_EINVAL);

    /* Must have not been registered */
    PJ_ASSERT_RETURN(mod_conference.id == -1, PJ_EINVALIDOP);

    /* Register to endpoint */
    status = pjsip_endpt_register_module(endpt, &mod_conference);
    if (status != PJ_SUCCESS)
	return status;

    // accept[0] = STR_APP_PIDF_XML;
    // accept[1] = STR_APP_XPIDF_XML;
    accept[0] = STR_APP_CONF_INFO_XML;

    /* Register event package to event module. */
    status = pjsip_evsub_register_pkg( &mod_conference, &STR_CONFERENCE, 
				       CONF_DEFAULT_EXPIRES, 
				       PJ_ARRAY_SIZE(accept), accept);
    if (status != PJ_SUCCESS) {
	pjsip_endpt_unregister_module(endpt, &mod_conference);
	return status;
    }

    return PJ_SUCCESS;
}


/*
 * Get conference module instance.
 */
PJ_DEF(pjsip_module*) pjsip_conf_instance(void)
{
    return &mod_conference;
}


/*
 * Create client subscription.
 */
PJ_DEF(pj_status_t) pjsip_conf_create_uac( pjsip_dialog *dlg,
					   const pjsip_evsub_user *user_cb,
					   unsigned options,
					   pjsip_evsub **p_evsub )
{
    pj_status_t status;
    pjsip_conf *conf;
    char obj_name[PJ_MAX_OBJ_NAME];
    pjsip_evsub *sub;

    PJ_ASSERT_RETURN(dlg && p_evsub, PJ_EINVAL);

    pjsip_dlg_inc_lock(dlg);

    /* Create event subscription */
    status = pjsip_evsub_create_uac( dlg,  &conf_user, &STR_CONFERENCE, 
				     options, &sub);
    if (status != PJ_SUCCESS)
	goto on_return;

    /* Create conference */
    conf = PJ_POOL_ZALLOC_T(dlg->pool, pjsip_conf);
    conf->dlg = dlg;
    conf->sub = sub;
    if (user_cb)
	pj_memcpy(&conf->user_cb, user_cb, sizeof(pjsip_evsub_user));

    pj_ansi_snprintf(obj_name, PJ_MAX_OBJ_NAME, "conf%p", dlg->pool);
    conf->conf_info_pool = pj_pool_create(dlg->pool->factory, obj_name, 
				       2048, 512, NULL);
    pj_ansi_snprintf(obj_name, PJ_MAX_OBJ_NAME, "tmconf%p", dlg->pool);
    conf->tmp_pool = pj_pool_create(dlg->pool->factory, obj_name, 
				    2048, 512, NULL);
	
	pj_ansi_snprintf(obj_name, PJ_MAX_OBJ_NAME, "g_conf%p", dlg->pool);
    g_conf_info_pool = pj_pool_create(dlg->pool->factory, obj_name, 
				    2048, 512, NULL);

	pj_memset(&g_conf_info_var, 0, sizeof(pjsip_conf_type));

    /* Attach to evsub */
    pjsip_evsub_set_mod_data(sub, mod_conference.id, conf);

    *p_evsub = sub;

on_return:
    pjsip_dlg_dec_lock(dlg);
    return status;
}


/*
 * Create server subscription.
 */
PJ_DEF(pj_status_t) pjsip_conf_create_uas( pjsip_dialog *dlg,
					   const pjsip_evsub_user *user_cb,
					   pjsip_rx_data *rdata,
					   pjsip_evsub **p_evsub )
{
    pjsip_accept_hdr *accept;
    pjsip_event_hdr *event;
    content_type_e content_type = CONTENT_TYPE_NONE;
    pjsip_evsub *sub;
    pjsip_conf *conf;
    char obj_name[PJ_MAX_OBJ_NAME];
    pj_status_t status;

    /* Check arguments */
    PJ_ASSERT_RETURN(dlg && rdata && p_evsub, PJ_EINVAL);

    /* Must be request message */
    PJ_ASSERT_RETURN(rdata->msg_info.msg->type == PJSIP_REQUEST_MSG,
		     PJSIP_ENOTREQUESTMSG);

    /* Check that request is SUBSCRIBE */
    PJ_ASSERT_RETURN(pjsip_method_cmp(&rdata->msg_info.msg->line.req.method,
				      &pjsip_subscribe_method)==0,
		     PJSIP_SIMPLE_ENOTCONFSUBSCRIBE);

    /* Check that Event header contains "conference" */
    event = (pjsip_event_hdr*)
    	    pjsip_msg_find_hdr_by_name(rdata->msg_info.msg, &STR_EVENT, NULL);
    if (!event) {
	return PJSIP_ERRNO_FROM_SIP_STATUS(PJSIP_SC_BAD_REQUEST);
    }
    if (pj_stricmp(&event->event_type, &STR_CONFERENCE) != 0) {
	return PJSIP_ERRNO_FROM_SIP_STATUS(PJSIP_SC_BAD_EVENT);
    }

    /* Check that request contains compatible Accept header. */
    accept = (pjsip_accept_hdr*)
    	     pjsip_msg_find_hdr(rdata->msg_info.msg, PJSIP_H_ACCEPT, NULL);
    if (accept) {
	unsigned i;
	for (i=0; i<accept->count; ++i) {
	    // if (pj_stricmp(&accept->values[i], &STR_APP_PIDF_XML)==0) {
        if (pj_stricmp(&accept->values[i], &STR_APP_CONF_INFO_XML)==0) {
		content_type = CONTENT_TYPE_CONFERENCE_INFO;
		break;
	    } 
        // else
	    // if (pj_stricmp(&accept->values[i], &STR_APP_XPIDF_XML)==0) {
		// content_type = CONTENT_TYPE_XPIDF;
		// break;
	    // }
	}

	if (i==accept->count) {
	    /* Nothing is acceptable */
	    return PJSIP_ERRNO_FROM_SIP_STATUS(PJSIP_SC_NOT_ACCEPTABLE);
	}

    } else {
	/* No Accept header.
	 * Treat as "application/conference-info+xml"
	 */
	content_type = CONTENT_TYPE_CONFERENCE_INFO;
    }

    /* Lock dialog */
    pjsip_dlg_inc_lock(dlg);


    /* Create server subscription */
    status = pjsip_evsub_create_uas( dlg, &conf_user, rdata, 0, &sub);
    if (status != PJ_SUCCESS)
	goto on_return;

    /* Create server conference subscription */
    conf = PJ_POOL_ZALLOC_T(dlg->pool, pjsip_conf);
    conf->dlg = dlg;
    conf->sub = sub;
    conf->content_type = content_type;
    if (user_cb)
	pj_memcpy(&conf->user_cb, user_cb, sizeof(pjsip_evsub_user));

    pj_ansi_snprintf(obj_name, PJ_MAX_OBJ_NAME, "conf%p", dlg->pool);
    conf->conf_info_pool = pj_pool_create(dlg->pool->factory, obj_name, 
				       512, 512, NULL);
    pj_ansi_snprintf(obj_name, PJ_MAX_OBJ_NAME, "tmconf%p", dlg->pool);
    conf->tmp_pool = pj_pool_create(dlg->pool->factory, obj_name, 
				    512, 512, NULL);

    /* Attach to evsub */
    pjsip_evsub_set_mod_data(sub, mod_conference.id, conf);

    /* Done: */
    *p_evsub = sub;

on_return:
    pjsip_dlg_dec_lock(dlg);
    return status;
}


/*
 * Forcefully terminate conference.
 */
PJ_DEF(pj_status_t) pjsip_conf_terminate( pjsip_evsub *sub,
					  pj_bool_t notify )
{
    return pjsip_evsub_terminate(sub, notify);
}

/*
 * Create SUBSCRIBE
 */
PJ_DEF(pj_status_t) pjsip_conf_initiate( pjsip_evsub *sub,
					 pj_int32_t expires,
					 pjsip_tx_data **p_tdata)
{
    return pjsip_evsub_initiate(sub, &pjsip_subscribe_method, expires, 
				p_tdata);
}


/*
 * Add custom headers.
 */
PJ_DEF(pj_status_t) pjsip_conf_add_header( pjsip_evsub *sub,
					   const pjsip_hdr *hdr_list )
{
    return pjsip_evsub_add_header( sub, hdr_list );
}


/*
 * Accept incoming subscription.
 */
PJ_DEF(pj_status_t) pjsip_conf_accept( pjsip_evsub *sub,
				       pjsip_rx_data *rdata,
				       int st_code,
				       const pjsip_hdr *hdr_list )
{
    return pjsip_evsub_accept( sub, rdata, st_code, hdr_list );
}


/*
 * Get conference info.
 */
PJ_DEF(pj_status_t) pjsip_conf_get_info( pjsip_evsub *sub,
					   pjsip_conf_type *conf_info )
{
    pjsip_conf *conf;

    PJ_ASSERT_RETURN(sub && conf_info, PJ_EINVAL);

    conf = (pjsip_conf*) pjsip_evsub_get_mod_data(sub, mod_conference.id);
    PJ_ASSERT_RETURN(conf!=NULL, PJSIP_SIMPLE_ENOCONFERENCE);

    if (conf->tmp_conf_info._is_valid) {
	PJ_ASSERT_RETURN(conf->tmp_pool!=NULL, PJSIP_SIMPLE_ENOCONFERENCE);
	pj_memcpy(conf_info, &conf->tmp_conf_info, sizeof(pjsip_conf_type));
    } else {
	PJ_ASSERT_RETURN(conf->conf_info_pool!=NULL, PJSIP_SIMPLE_ENOCONFERENCE);
	pj_memcpy(conf_info, &conf->conf_info, sizeof(pjsip_conf_type));
    }

    return PJ_SUCCESS;
}

/*
 * Set conference status.
 */
// PJ_DEF(pj_status_t) pjsip_conf_set_status( pjsip_evsub *sub,
// 					   const pjsip_conf_status *status )
PJ_DEF(pj_status_t) pjsip_conf_set_info( pjsip_evsub *sub,
					   const pjsip_conf_type *conf_info )
{
    unsigned i;
    pj_pool_t *tmp;
    pjsip_conf *conf;

    PJ_ASSERT_RETURN(sub && conf_info, PJ_EINVAL);

    conf = (pjsip_conf*) pjsip_evsub_get_mod_data(sub, mod_conference.id);
    PJ_ASSERT_RETURN(conf!=NULL, PJSIP_SIMPLE_ENOCONFERENCE);

    // for (i=0; i<status->info_cnt; ++i) {
	// conf->status.info[i].basic_open = status->info[i].basic_open;
	// if (conf->status.info[i].id.slen) {
	//     /* Id already set */
	// } else if (status->info[i].id.slen == 0) {
	//     pj_create_unique_string(conf->dlg->pool, 
	//     			    &conf->status.info[i].id);
	// } else {
	//     pj_strdup(conf->dlg->pool, 
	// 	      &conf->status.info[i].id,
	// 	      &status->info[i].id);
	// }
	// pj_strdup(conf->tmp_pool, 
	// 	  &conf->status.info[i].contact,
	// 	  &status->info[i].contact);

	// /* Duplicate <person> */
	// // conf->status.info[i].rpid.activity = 
	// //     status->info[i].rpid.activity;
	// // pj_strdup(conf->tmp_pool, 
	// // 	  &conf->status.info[i].rpid.id,
	// // 	  &status->info[i].rpid.id);
	// // pj_strdup(conf->tmp_pool,
	// // 	  &conf->status.info[i].rpid.note,
	// // 	  &status->info[i].rpid.note);

    // }

    // conf->status.info_cnt = status->info_cnt;

    /* Swap pools */
    tmp = conf->tmp_pool;
    conf->tmp_pool = conf->conf_info_pool;
    conf->conf_info_pool = tmp;
    pj_pool_reset(conf->tmp_pool);

    return PJ_SUCCESS;
}


/*
 * Create message body.
 */
static pj_status_t conf_create_msg_body( pjsip_conf *conf, 
					 pjsip_tx_data *tdata)
{
    pj_str_t entity;

    /* Get publisher URI */
    entity.ptr = (char*) pj_pool_alloc(tdata->pool, PJSIP_MAX_URL_SIZE);
    entity.slen = pjsip_uri_print(PJSIP_URI_IN_REQ_URI,
				  conf->dlg->local.info->uri,
				  entity.ptr, PJSIP_MAX_URL_SIZE);
    if (entity.slen < 1)
	return PJ_ENOMEM;

    if (conf->content_type == CONTENT_TYPE_CONFERENCE_INFO) {

	return pjsip_conf_create_confinfo(tdata->pool, &conf->conf_info,
				      &entity, &tdata->msg->body);

    } 
    // else if (conf->content_type == CONTENT_TYPE_XPIDF) {

	// return pjsip_conf_create_xpidf(tdata->pool, &conf->status,
	// 			       &entity, &tdata->msg->body);

    // } 
    else {
	return PJSIP_SIMPLE_EBADCONFCONTENT;
    }
}


/*
 * Create NOTIFY
 */
PJ_DEF(pj_status_t) pjsip_conf_notify( pjsip_evsub *sub,
				       pjsip_evsub_state state,
				       const pj_str_t *state_str,
				       const pj_str_t *reason,
				       pjsip_tx_data **p_tdata)
{
//     pjsip_conf *conf;
//     pjsip_tx_data *tdata;
//     pj_status_t status;
    
//     /* Check arguments. */
//     PJ_ASSERT_RETURN(sub, PJ_EINVAL);

//     /* Get the conference object. */
//     conf = (pjsip_conf*) pjsip_evsub_get_mod_data(sub, mod_conference.id);
//     PJ_ASSERT_RETURN(conf != NULL, PJSIP_SIMPLE_ENOCONFERENCE);

//     /* Must have at least one conference info, unless state is 
//      * PJSIP_EVSUB_STATE_TERMINATED. This could happen if subscription
//      * has not been active (e.g. we're waiting for user authorization)
//      * and remote cancels the subscription.
//      */
//     PJ_ASSERT_RETURN(state==PJSIP_EVSUB_STATE_TERMINATED ||
// 		     conf->status.info_cnt > 0, PJSIP_SIMPLE_ENOCONFERENCEINFO);


//     /* Lock object. */
//     pjsip_dlg_inc_lock(conf->dlg);

//     /* Create the NOTIFY request. */
//     status = pjsip_evsub_notify( sub, state, state_str, reason, &tdata);
//     if (status != PJ_SUCCESS)
// 	goto on_return;


//     /* Create message body to reflect the conference status. 
//      * Only do this if we have conference status info to send (see above).
//      */
//     if (conf->status.info_cnt > 0) {
// 	status = conf_create_msg_body( conf, tdata );
// 	if (status != PJ_SUCCESS)
// 	    goto on_return;
//     }

//     /* Done. */
//     *p_tdata = tdata;


// on_return:
//     pjsip_dlg_dec_lock(conf->dlg);
//     return status;

    return PJ_FALSE;
}


/*
 * Create NOTIFY that reflect current state.
 */
PJ_DEF(pj_status_t) pjsip_conf_current_notify( pjsip_evsub *sub,
					       pjsip_tx_data **p_tdata )
{
//     pjsip_conf *conf;
//     pjsip_tx_data *tdata;
//     pj_status_t status;
    
//     /* Check arguments. */
//     PJ_ASSERT_RETURN(sub, PJ_EINVAL);

//     /* Get the conference object. */
//     conf = (pjsip_conf*) pjsip_evsub_get_mod_data(sub, mod_conference.id);
//     PJ_ASSERT_RETURN(conf != NULL, PJSIP_SIMPLE_ENOCONFERENCE);

//     /* We may not have a conference info yet, e.g. when we receive SUBSCRIBE
//      * to refresh subscription while we're waiting for user authorization.
//      */
//     //PJ_ASSERT_RETURN(conf->status.info_cnt > 0, 
//     //		       PJSIP_SIMPLE_ENOCONFERENCEINFO);


//     /* Lock object. */
//     pjsip_dlg_inc_lock(conf->dlg);

//     /* Create the NOTIFY request. */
//     status = pjsip_evsub_current_notify( sub, &tdata);
//     if (status != PJ_SUCCESS)
// 	goto on_return;


//     /* Create message body to reflect the conference status. */
//     if (conf->status.info_cnt > 0) {
// 	status = conf_create_msg_body( conf, tdata );
// 	if (status != PJ_SUCCESS)
// 	    goto on_return;
//     }

//     /* Done. */
//     *p_tdata = tdata;


// on_return:
//     pjsip_dlg_dec_lock(conf->dlg);
//     return status;

    return PJ_FALSE;
}


/*
 * Send request.
 */
PJ_DEF(pj_status_t) pjsip_conf_send_request( pjsip_evsub *sub,
					     pjsip_tx_data *tdata )
{
    return pjsip_evsub_send_request(sub, tdata);
}


/*
 * This callback is called by event subscription when subscription
 * state has changed.
 */
static void conf_on_evsub_state( pjsip_evsub *sub, pjsip_event *event)
{
    pjsip_conf *conf;

    conf = (pjsip_conf*) pjsip_evsub_get_mod_data(sub, mod_conference.id);
    PJ_ASSERT_ON_FAIL(conf!=NULL, {return;});

    if (conf->user_cb.on_evsub_state)
	(*conf->user_cb.on_evsub_state)(sub, event);

    if (pjsip_evsub_get_state(sub) == PJSIP_EVSUB_STATE_TERMINATED) {
	if (conf->conf_info_pool) {
	    pj_pool_release(conf->conf_info_pool);
	    conf->conf_info_pool = NULL;
	}
	if (conf->tmp_pool) {
	    pj_pool_release(conf->tmp_pool);
	    conf->tmp_pool = NULL;
	}
	if (g_conf_info_pool) {
		pj_pool_release(g_conf_info_pool);
		g_conf_info_pool = NULL;

		pj_memset(&g_conf_info_var, 0, sizeof(pjsip_conf_type));
	}
	
    }
}

/*
 * Called when transaction state has changed.
 */
static void conf_on_evsub_tsx_state( pjsip_evsub *sub, pjsip_transaction *tsx,
				     pjsip_event *event)
{
    pjsip_conf *conf;

    conf = (pjsip_conf*) pjsip_evsub_get_mod_data(sub, mod_conference.id);
    PJ_ASSERT_ON_FAIL(conf!=NULL, {return;});

    if (conf->user_cb.on_tsx_state)
	(*conf->user_cb.on_tsx_state)(sub, tsx, event);
}


/*
 * Called when SUBSCRIBE is received.
 */
static void conf_on_evsub_rx_refresh( pjsip_evsub *sub, 
				      pjsip_rx_data *rdata,
				      int *p_st_code,
				      pj_str_t **p_st_text,
				      pjsip_hdr *res_hdr,
				      pjsip_msg_body **p_body)
{
    pjsip_conf *conf;

    conf = (pjsip_conf*) pjsip_evsub_get_mod_data(sub, mod_conference.id);
    PJ_ASSERT_ON_FAIL(conf!=NULL, {return;});

    if (conf->user_cb.on_rx_refresh) {
	(*conf->user_cb.on_rx_refresh)(sub, rdata, p_st_code, p_st_text,
				       res_hdr, p_body);

    } else {
	/* Implementors MUST send NOTIFY if it implements on_rx_refresh */
	pjsip_tx_data *tdata;
	pj_str_t timeout = { "timeout", 7};
	pj_status_t status;

	if (pjsip_evsub_get_state(sub)==PJSIP_EVSUB_STATE_TERMINATED) {
	    status = pjsip_conf_notify( sub, PJSIP_EVSUB_STATE_TERMINATED,
					NULL, &timeout, &tdata);
	} else {
	    status = pjsip_conf_current_notify(sub, &tdata);
	}

	if (status == PJ_SUCCESS)
	    pjsip_conf_send_request(sub, tdata);
    }
}


/*
 * Process the content of incoming NOTIFY request and update temporary
 * status.
 *
 * return PJ_SUCCESS if incoming request is acceptable. If return value
 *	  is not PJ_SUCCESS, res_hdr may be added with Warning header.
 */
static pj_status_t conf_process_rx_notify( pjsip_conf *conf,
					   pjsip_rx_data *rdata, 
					   int *p_st_code,
					   pj_str_t **p_st_text,
					   pjsip_hdr *res_hdr)
{
    const pj_str_t STR_MULTIPART = { "multipart", 9 };
    pjsip_ctype_hdr *ctype_hdr;
    pj_status_t status = PJ_SUCCESS;

    *p_st_text = NULL;

    /* Check Content-Type and msg body are confent. */
    ctype_hdr = rdata->msg_info.ctype;

    if (ctype_hdr==NULL || rdata->msg_info.msg->body==NULL) {
	
	pjsip_warning_hdr *warn_hdr;
	pj_str_t warn_text;

	*p_st_code = PJSIP_SC_BAD_REQUEST;

	warn_text = pj_str("Message body is not confent");
	warn_hdr = pjsip_warning_hdr_create(rdata->tp_info.pool, 399,
					    pjsip_endpt_name(conf->dlg->endpt),
					    &warn_text);
	pj_list_push_back(res_hdr, warn_hdr);

	return PJSIP_ERRNO_FROM_SIP_STATUS(PJSIP_SC_BAD_REQUEST);
    }

    /* Parse content. */
    if (pj_stricmp(&ctype_hdr->media.type, &STR_MULTIPART)==0) {
	pjsip_multipart_part *mpart;
	pjsip_media_type ctype;

	// pjsip_media_type_init(&ctype, (pj_str_t*)&STR_APPLICATION,
	// 		      (pj_str_t*)&STR_PIDF_XML);
	pjsip_media_type_init(&ctype, (pj_str_t*)&STR_APPLICATION,
			      (pj_str_t*)&STR_CONF_INFO_XML);
	mpart = pjsip_multipart_find_part(rdata->msg_info.msg->body,
					  &ctype, NULL);
	if (mpart) {
	    // status = pjsip_conf_parse_confinfo2((char*)mpart->body->data,
		// 			    mpart->body->len, conf->tmp_pool,
		// 			    &conf->tmp_status);

        status = pjsip_conf_parse_confinfo2((char*)mpart->body->data,
                mpart->body->len, conf->tmp_pool,
                &conf->tmp_conf_info);

	}

	// if (mpart==NULL) {
	//     pjsip_media_type_init(&ctype, (pj_str_t*)&STR_APPLICATION,
	// 			  (pj_str_t*)&STR_XPIDF_XML);
	//     mpart = pjsip_multipart_find_part(rdata->msg_info.msg->body,
	// 				      &ctype, NULL);
	//     if (mpart) {
	// 	status = pjsip_conf_parse_xpidf2((char*)mpart->body->data,
	// 					 mpart->body->len,
	// 					 conf->tmp_pool,
	// 					 &conf->tmp_status);
	//     } else {
	// 	status = PJSIP_SIMPLE_EBADCONTENT;
	//     }
	// }
    }
    else
    if (pj_stricmp(&ctype_hdr->media.type, &STR_APPLICATION)==0 &&
	// pj_stricmp(&ctype_hdr->media.subtype, &STR_PIDF_XML)==0)
    pj_stricmp(&ctype_hdr->media.subtype, &STR_CONF_INFO_XML)==0)
    {
	// status = pjsip_conf_parse_confinfo( rdata, conf->tmp_pool,
	// 				&conf->tmp_status);

    	status = pjsip_conf_parse_confinfo( rdata, conf->tmp_pool,
					&conf->tmp_conf_info);

    }
    // else 
    // if (pj_stricmp(&ctype_hdr->media.type, &STR_APPLICATION)==0 &&
	// pj_stricmp(&ctype_hdr->media.subtype, &STR_XPIDF_XML)==0)
    // {
	// status = pjsip_conf_parse_xpidf( rdata, conf->tmp_pool,
	// 				 &conf->tmp_status);
    // }
    else
    {
	status = PJSIP_SIMPLE_EBADCONFCONTENT;
    }

    if (status != PJ_SUCCESS) {
	/* Unsupported or bad Content-Type */
	if (PJSIP_CONF_BAD_CONTENT_RESPONSE >= 300) {
	    pjsip_accept_hdr *accept_hdr;
	    pjsip_warning_hdr *warn_hdr;

	    *p_st_code = PJSIP_CONF_BAD_CONTENT_RESPONSE;

	    /* Add Accept header */
	    accept_hdr = pjsip_accept_hdr_create(rdata->tp_info.pool);
	    // accept_hdr->values[accept_hdr->count++] = STR_APP_PIDF_XML;
	    // accept_hdr->values[accept_hdr->count++] = STR_APP_XPIDF_XML;
        accept_hdr->values[accept_hdr->count++] = STR_APP_CONF_INFO_XML;
	    pj_list_push_back(res_hdr, accept_hdr);

	    /* Add Warning header */
	    warn_hdr = pjsip_warning_hdr_create_from_status(
					rdata->tp_info.pool,
					pjsip_endpt_name(conf->dlg->endpt),
					status);
	    pj_list_push_back(res_hdr, warn_hdr);

	    return status;
	} else {
	    pj_assert(PJSIP_CONF_BAD_CONTENT_RESPONSE/100 == 2);
	    PJ_PERROR(4,(THIS_FILE, status,
			 "Ignoring conference error due to "
		         "PJSIP_CONF_BAD_CONTENT_RESPONSE setting [%d]",
		         PJSIP_CONF_BAD_CONTENT_RESPONSE));
	    *p_st_code = PJSIP_CONF_BAD_CONTENT_RESPONSE;
	    status = PJ_SUCCESS;
	}
    }

    /* If application calls pjsip_conf_get_info(), redirect the call to
     * retrieve the temporary status.
     */
    conf->tmp_conf_info._is_valid = PJ_TRUE;

    return PJ_SUCCESS;
}


/*
 * Called when NOTIFY is received.
 */
static void conf_on_evsub_rx_notify( pjsip_evsub *sub, 
				     pjsip_rx_data *rdata,
				     int *p_st_code,
				     pj_str_t **p_st_text,
				     pjsip_hdr *res_hdr,
				     pjsip_msg_body **p_body)
{
    pjsip_conf *conf;
    pj_status_t status;

    conf = (pjsip_conf*) pjsip_evsub_get_mod_data(sub, mod_conference.id);
    PJ_ASSERT_ON_FAIL(conf!=NULL, {return;});

    if (rdata->msg_info.msg->body) {
	status = conf_process_rx_notify( conf, rdata, p_st_code, p_st_text,
					 res_hdr );
	if (status != PJ_SUCCESS)
	    return;

    } else {
#if 1
	/* This is the newest change, http://trac.pjsip.org/repos/ticket/873
	 * Some app want to be notified about the empty NOTIFY, e.g. to 
	 * decide whether it should consider the conference as offline.
	 * In this case, leave the conference state unchanged, but set the
	 * "tuple_node" in pjsip_conf_status to NULL.
	 */
	// unsigned i;
	// for (i=0; i<conf->status.info_cnt; ++i) {
	//     conf->status.info[i].tuple_node = NULL;
	// }

#elif 0
	/* This has just been changed. Previously, we treat incoming NOTIFY
	 * with no message body as having the conference subscription closed.
	 * Now we treat it as no change in conference status (ref: EyeBeam).
	 */
	*p_st_code = 200;
	return;
#else
	unsigned i;
	/* Subscription is terminated. Consider contact is offline */
	conf->tmp_conf_info._is_valid = PJ_TRUE;
	for (i=0; i<conf->tmp_status.info_cnt; ++i)
	    conf->tmp_status.info[i].basic_open = PJ_FALSE;
#endif
    }

    /* Notify application. */
    if (conf->user_cb.on_rx_notify) {
	(*conf->user_cb.on_rx_notify)(sub, rdata, p_st_code, p_st_text, 
				      res_hdr, p_body);
    }

    
    /* If application responded NOTIFY with 2xx, copy temporary status
     * to main status, and mark the temporary status as invalid.
     */
    if ((*p_st_code)/100 == 2) {
	pj_pool_t *tmp;

	pj_memcpy(&conf->conf_info, &conf->tmp_conf_info, sizeof(pjsip_conf_type));

	/* Swap the pool */
	tmp = conf->tmp_pool;
	conf->tmp_pool = conf->conf_info_pool;
	conf->conf_info_pool = tmp;
    }

    conf->tmp_conf_info._is_valid = PJ_FALSE;
    pj_pool_reset(conf->tmp_pool);

    /* Done */
}

/*
 * Called when it's time to send SUBSCRIBE.
 */
static void conf_on_evsub_client_refresh(pjsip_evsub *sub)
{
    pjsip_conf *conf;

    conf = (pjsip_conf*) pjsip_evsub_get_mod_data(sub, mod_conference.id);
    PJ_ASSERT_ON_FAIL(conf!=NULL, {return;});

    if (conf->user_cb.on_client_refresh) {
	(*conf->user_cb.on_client_refresh)(sub);
    } else {
	pj_status_t status;
	pjsip_tx_data *tdata;

	status = pjsip_conf_initiate(sub, -1, &tdata);
	if (status == PJ_SUCCESS)
	    pjsip_conf_send_request(sub, tdata);
    }
}

/*
 * Called when no refresh is received after the interval.
 */
static void conf_on_evsub_server_timeout(pjsip_evsub *sub)
{
    pjsip_conf *conf;

    conf = (pjsip_conf*) pjsip_evsub_get_mod_data(sub, mod_conference.id);
    PJ_ASSERT_ON_FAIL(conf!=NULL, {return;});

    if (conf->user_cb.on_server_timeout) {
	(*conf->user_cb.on_server_timeout)(sub);
    } else {
	pj_status_t status;
	pjsip_tx_data *tdata;
	pj_str_t reason = { "timeout", 7 };

	status = pjsip_conf_notify(sub, PJSIP_EVSUB_STATE_TERMINATED,
				   NULL, &reason, &tdata);
	if (status == PJ_SUCCESS)
	    pjsip_conf_send_request(sub, tdata);
    }
}

