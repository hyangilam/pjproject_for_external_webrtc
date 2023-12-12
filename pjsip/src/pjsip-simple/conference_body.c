/* $Id: presence_body.c 5701 2017-11-22 06:59:47Z riza $ */
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
#include <pjsip/sip_msg.h>
#include <pjsip/sip_transport.h>
#include <pj/guid.h>
#include <pj/log.h>
#include <pj/os.h>
#include <pj/pool.h>
#include <pj/string.h>


// nugucall - conference

#define THIS_FILE   "conference_body.c"


static const pj_str_t STR_APPLICATION = { "application", 11 };
static const pj_str_t STR_CONF_INFO_XML =	{ "conference-info+xml", 19 };

//
// static const pj_str_t STR_
// static const pj_str_t STR_PIDF_XML =	{ "pidf+xml", 8 };
// static const pj_str_t STR_XPIDF_XML =	{ "xpidf+xml", 9 };


/*
 * Function to print XML message body.
 */
static int conf_print_body(struct pjsip_msg_body *msg_body, 
			   char *buf, pj_size_t size)
{
    return pj_xml_print((const pj_xml_node*)msg_body->data, buf, size, 
    			PJ_TRUE);
}


/*
 * Function to clone XML document.
 */
static void* xml_clone_data(pj_pool_t *pool, const void *data, unsigned len)
{
    PJ_UNUSED_ARG(len);
    return pj_xml_clone( pool, (const pj_xml_node*) data);
}


/*
 * This is a utility function to create PIDF message body from PJSIP
 * presence status (pjsip_conf_status).
 */
PJ_DEF(pj_status_t) pjsip_conf_create_confinfo( pj_pool_t *pool,
					    const pjsip_conf_type *conf_info,
					    const pj_str_t *entity,
					    pjsip_msg_body **p_body )
{
//     pjconfinfo_xml_node_conf *confinfo;
//     pjsip_msg_body *body;
//     unsigned i;

//     /* Create <presence>. */
//     confinfo = pjconfinfo_create(pool, entity);

//     /* Create <tuple> */
//     for (i=0; i<status->info_cnt; ++i) {

// 	pjconfinfo_xml_node_tuple *confinfo_tuple;
// 	pjconfinfo_xml_node_status *confinfo_status;
// 	pj_str_t id;

// 	/* Add tuple id. */
// 	if (status->info[i].id.slen == 0) {
// 	    /* xs:ID must start with letter */
// 	    //pj_create_unique_string(pool, &id);
// 	    id.ptr = (char*)pj_pool_alloc(pool, pj_GUID_STRING_LENGTH()+2);
// 	    id.ptr += 2;
// 	    pj_generate_unique_string(&id);
// 	    id.ptr -= 2;
// 	    id.ptr[0] = 'p';
// 	    id.ptr[1] = 'j';
// 	    id.slen += 2;
// 	} else {
// 	    id = status->info[i].id;
// 	}

// 	confinfo_tuple = pjconfinfo_add_tuple(pool, confinfo, &id);

// 	/* Set <contact> */
// 	if (status->info[i].contact.slen)
// 	    pjconfinfo_tuple_set_contact(pool, confinfo_tuple, 
// 				     &status->info[i].contact);


// 	/* Set basic status */
// 	confinfo_status = pjconfinfo_tuple_get_status(confinfo_tuple);
// 	pjconfinfo_status_set_basic_open(confinfo_status, 
// 				     status->info[i].basic_open);

// 	/* Add <timestamp> if configured */
// #if defined(PJSIP_CONF_INFO_ADD_TIMESTAMP) && PJSIP_CONF_INFO_ADD_TIMESTAMP
// 	if (PJSIP_CONF_INFO_ADD_TIMESTAMP) {
// 	  char buf[50];
// 	  int tslen = 0;
// 	  pj_time_val tv;
// 	  pj_parsed_time pt;

// 	  pj_gettimeofday(&tv);
// 	  /* TODO: convert time to GMT! (unsupported by pjlib) */
// 	  pj_time_decode( &tv, &pt);

// 	  tslen = pj_ansi_snprintf(buf, sizeof(buf),
// 				   "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ",
// 				   pt.year, pt.mon+1, pt.day, 
// 				   pt.hour, pt.min, pt.sec, pt.msec);
// 	  if (tslen > 0 && tslen < (int)sizeof(buf)) {
// 	      pj_str_t time = pj_str(buf);
// 	      pjconfinfo_tuple_set_timestamp(pool, confinfo_tuple, &time);
// 	  }
// 	}
// #endif
//     }

//     /* Create <person> (RPID) */
//     // if (status->info_cnt) {
// 	// pjrpid_add_element(pidf, pool, 0, &status->info[0].rpid);
//     // }

//     body = PJ_POOL_ZALLOC_T(pool, pjsip_msg_body);
//     body->data = confinfo;
//     body->content_type.type = STR_APPLICATION;
//     // body->content_type.subtype = STR_PIDF_XML;
// 	body->content_type.subtype = STR_CONF_INFO_XML;
//     body->print_body = &conf_print_body;
//     body->clone_data = &xml_clone_data;

//     *p_body = body;

    // return PJ_SUCCESS;    

	return PJ_FALSE;
}


// /*
//  * This is a utility function to create X-PIDF message body from PJSIP
//  * presence status (pjsip_conf_status).
//  */
// PJ_DEF(pj_status_t) pjsip_conf_create_xpidf( pj_pool_t *pool,
// 					     const pjsip_conf_status *status,
// 					     const pj_str_t *entity,
// 					     pjsip_msg_body **p_body )
// {
//     /* Note: PJSIP implementation of XPIDF is not complete!
//      */
//     pjxpidf_conf *xpidf;
//     pjsip_msg_body *body;

//     PJ_LOG(4,(THIS_FILE, "Warning: XPIDF format is not fully supported "
// 			 "by PJSIP"));

//     /* Create XPIDF document. */
//     xpidf = pjxpidf_create(pool, entity);

//     /* Set basic status. */
//     if (status->info_cnt > 0)
// 	pjxpidf_set_status( xpidf, status->info[0].basic_open);
//     else
// 	pjxpidf_set_status( xpidf, PJ_FALSE);

//     body = PJ_POOL_ZALLOC_T(pool, pjsip_msg_body);
//     body->data = xpidf;
//     body->content_type.type = STR_APPLICATION;
//     body->content_type.subtype = STR_XPIDF_XML;
//     body->print_body = &conf_print_body;
//     body->clone_data = &xml_clone_data;

//     *p_body = body;

//     return PJ_SUCCESS;
// }



/*
 * This is a utility function to parse PIDF body into PJSIP presence status.
 */
PJ_DEF(pj_status_t) pjsip_conf_parse_confinfo( pjsip_rx_data *rdata,
					   pj_pool_t *pool,
					   pjsip_conf_type *conf_info)
{
    return pjsip_conf_parse_confinfo2((char*)rdata->msg_info.msg->body->data,
				  rdata->msg_info.msg->body->len,
				  pool, conf_info);

}



PJ_DEF(pj_status_t) pjsip_conf_parse_confinfo2(char *body, unsigned body_len,
					   pj_pool_t *pool,
					   pjsip_conf_type *conf_info)
{
	pj_status_t status = PJ_SUCCESS;
    pjconfinfo_xml_node_conf *confinfo_root_element;			// xml node

// nugucall - conference-info simple
#if 0
	g_conf_info_var._nReceived++;
	PJ_LOG(5,(THIS_FILE, "conference-info nReceived: %d", g_conf_info_var._nReceived));

    confinfo_root_element = pjconfinfo_parse(pool, body, body_len);
    if (confinfo_root_element == NULL){
		status = PJSIP_SIMPLE_EBADCONFERENCEINFO;
		goto out;
	}
	
	status = pjconfinfo_parse_confinfo_attr(pool, confinfo_root_element, conf_info);
	if(status != PJ_SUCCESS)
		goto out;


	if(g_conf_info_var._nReceived == 1) {
		if(conf_info->state != PJSIP_CONF_STATE_FULL) {
			PJ_LOG(5,(THIS_FILE, "First conference-info is not full state"));
			status = PJSIP_SIMPLE_EBADCONFERENCEINFO;
			goto out;
		}
		g_conf_info_var.version = conf_info->version;
	}
	else if(g_conf_info_var._nReceived > 1) {
		/*
		Each time a new NOTIFY is received, the value of the local version
		number and the value of the 'version' attribute in the new received
		document are compared.  If the value in the document is equal to or
		less than the local version, the document is discarded without
		processing.
		*/
		if(conf_info->version - g_conf_info_var.version <= 0) {
			PJ_LOG(5, (THIS_FILE, 
			"new received version[%d], local version[%d] this conference-info discarded", 
			conf_info->version, g_conf_info_var.version));
			status = PJSIP_SIMPLE_EBADCONFERENCEINFOVERSION;
			goto out;
		}
	}

	if(conf_info->state == PJSIP_CONF_STATE_DELETED) {
		/*
			A parent element with
			"deleted" 'state' SHOULD NOT contain child elements.  Any information
			provided for child elements of a "deleted" parent MUST be ignored by
			the package subscriber. 
		*/
		pj_pool_reset(g_conf_info_pool);
		pj_memset(&g_conf_info_var, 0, sizeof(pjsip_conf_type));
		g_conf_info_var.version = conf_info->version;
		/*
			Otherwise, if the received NOTIFY contains a "full" or "deleted"
			state, the conference package subscriber MUST set the local 'version'
			number to the value of the 'version' attribute from the received
			<conference-info> and replace the local information with the received
			document.  
			Receiving "deleted" state for the <conference-info>
			element means that the conference has ceased to exist and the
			subscriber SHOULD terminate the subscription by sending the SUBSCRIBE
			with Expires = 0.
		*/

		// Do Something
		

	} else if(conf_info->state == PJSIP_CONF_STATE_PARTIAL) {
		// If the parent's 'state' is "partial", the state of its children MAY be either "partial", "full", or "deleted". 
		/*
			 if
			the 'version' number in the received document is more than one number
			higher than the previous local version number, the subscriber MUST
			generate a subscription refresh request to trigger a full state
			notification.  If the 'version' number in the document is one higher
			than the local version number, the local version number is updated
			accordingly and the document is used to update the local content as
			described below.
		*/

		if(conf_info->version - g_conf_info_var.version > 1) {
			/*
				the subscriber MUST
				generate a subscription refresh request to trigger a full state
				notification.
			*/

			// call - conf_on_evsub_client_refresh ????
		} else if(conf_info->version - g_conf_info_var.version == 1) {
			// Do partial update

		} else if(conf_info->version - g_conf_info_var.version == 0) {
			/*
			Note that a partial
			notification and a subsequent full notification over the same
			dialog MAY contain the same version number if no change in the
			conference state occurred in between.
			*/
			// same as before
			goto out;

		} else {
			// ???
		}

	} else if(conf_info->state == PJSIP_CONF_STATE_FULL) {
		// A "full" conference document MUST at least include the <conference-description> and <users> child elements.
		// if the parent's 'state' is "full", the state of its children MUST be "full".

		pj_memset(&g_conf_info_var, 0, sizeof(pjsip_conf_type));

		/*
			Otherwise, if the received NOTIFY contains a "full" or "deleted"
			state, the conference package subscriber MUST set the local 'version'
			number to the value of the 'version' attribute from the received
			<conference-info> and replace the local information with the received
			document.  
		*/
		g_conf_info_var.version = conf_info->version;

		status = pjconfinfo_parse_confinfo_elem(pool, confinfo_root_element, conf_info);
	}




    // conf_status->info_cnt = 0;

    // confinfo_tuple = pjconfinfo_get_first_tuple(confinfo);
    // while (confinfo_tuple && conf_status->info_cnt < PJSIP_CONF_MAX_INFO) {
	// pjconfinfo_xml_node_status *confinfo_status;

	// conf_status->info[conf_status->info_cnt].tuple_node = 
	//     pj_xml_clone(pool, confinfo_tuple);

	// pj_strdup(pool, 
	// 	  &conf_status->info[conf_status->info_cnt].id,
	// 	  pjconfinfo_tuple_get_id(confinfo_tuple));

	// pj_strdup(pool, 
	// 	  &conf_status->info[conf_status->info_cnt].contact,
	// 	  pjconfinfo_tuple_get_contact(confinfo_tuple));

	// confinfo_status = pjconfinfo_tuple_get_status(confinfo_tuple);
	// if (confinfo_status) {
	//     conf_status->info[conf_status->info_cnt].basic_open = 
	// 	pjconfinfo_status_is_basic_open(confinfo_status);
	// } else {
	//     conf_status->info[conf_status->info_cnt].basic_open = PJ_FALSE;
	// }

	// confinfo_tuple = pjconfinfo_get_next_tuple( confinfo, confinfo_tuple );
	// conf_status->info_cnt++;
    // }

    /* Parse <person> (RPID) */
    // pjrpid_get_element(confinfo, pool, &conf_status->info[0].rpid);
out:
    return status;
#else 

    confinfo_root_element = pjconfinfo_parse(pool, body, body_len);
    if (confinfo_root_element == NULL){
		status = PJSIP_SIMPLE_EBADCONFERENCEINFO;
		goto out;
	}

	pj_memset(&g_conf_info_var, 0, sizeof(pjsip_conf_type));

	status = pjconfinfo_parse_confinfo_elem(pool, confinfo_root_element, conf_info);

out:
    return status;
#endif
}
