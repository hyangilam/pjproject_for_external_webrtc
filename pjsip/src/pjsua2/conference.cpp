/* $Id: conference.cpp 4704 2014-01-16 05:30:46Z ming $ */
/*
 * Copyright (C) 2013 Teluu Inc. (http://www.teluu.com)
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
#include <pjsua2/conference.hpp>
#include <pjsua2/account.hpp>
#include "util.hpp"

// nugucall - conference

using namespace pj;
using namespace std;

#define THIS_FILE		"conference.cpp"


///////////////////////////////////////////////////////////////////////////////

// ConferenceStatus::ConferenceStatus()
// // : status(PJSUA_CONFERENCE_STATUS_UNKNOWN), activity(PJRPID_ACTIVITY_UNKNOWN)
// : status(PJSUA_CONFERENCE_STATUS_UNKNOWN)
// {
// }


///////////////////////////////////////////////////////////////////////////////

void ConferenceConfig::readObject(const ContainerNode &node) throw(Error)
{
    ContainerNode this_node = node.readContainer("ConferenceConfig");

    NODE_READ_STRING   ( this_node, uri);
    NODE_READ_BOOL     ( this_node, subscribe);
}

void ConferenceConfig::writeObject(ContainerNode &node) const throw(Error)
{
    ContainerNode this_node = node.writeNewContainer("ConferenceConfig");

    NODE_WRITE_STRING  ( this_node, uri);
    NODE_WRITE_BOOL    ( this_node, subscribe);
}

//////////////////////////////////////////////////////////////////////////////

void ConferenceInfo::fromPj(const pjsua_conference_info &pbi)
{
    uri 		= pj2Str(pbi.uri);
    contact 		= pj2Str(pbi.contact);
    confMonitorEnabled 	= PJ2BOOL(pbi.monitor_conf);
    subState 		= pbi.sub_state;
    subStateName 	= string(pbi.sub_state_name);
    subTermCode 	= (pjsip_status_code)pbi.sub_term_code;
    subTermReason 	= pj2Str(pbi.sub_term_reason);
    
    /* Conference status */
    // confStatus.status	= pbi.status;
    
    // confStatus.statusText = pj2Str(pbi.status_text);
    // confStatus.activity = pbi.rpid.activity;
    // confStatus.note	= pj2Str(pbi.rpid.note);
    // confStatus.rpidId	= pj2Str(pbi.rpid.id);


    if(pbi.conf_info._is_valid){
        infoUpdated = true;
#if 0 // nugucall - conference-info simple
        conf_info_version = pbi.conf_info.version;
        conf_info_entity = pj2Str(pbi.conf_info.entity);
        conf_desc_display_text = pj2Str(pbi.conf_info.conf_description.display_text);
        conf_state_user_count = pbi.conf_info.conf_state.user_count;
        conf_state_active = pbi.conf_info.conf_state.active;

        if(conf_state_user_count > 0){
            pjsip_conf_user *cur_user = pbi.conf_info.users.user_list->next;
            const pjsip_conf_user *user_list_head = pbi.conf_info.users.user_list;
            while(cur_user != user_list_head)
            {
                ConferenceUser user;
                user.entity = pj2Str(cur_user->user.entity);
                user.display_text = pj2Str(cur_user->user.display_text);

                pjsip_conf_endpoint *cur_endpoint = cur_user->user.endpoint_list.next;
                while(cur_endpoint != &cur_user->user.endpoint_list){
                    ConferenceEndpoint endpoint;
                    endpoint.entity = pj2Str(cur_endpoint->endpoint.entity);
                    endpoint.display_text = pj2Str(cur_endpoint->endpoint.display_text);
                    endpoint.status = cur_endpoint->endpoint.status;

                    pjsip_conf_media *cur_media = cur_endpoint->endpoint.media_list.next;
                    while(cur_media != &cur_endpoint->endpoint.media_list){
                        ConferenceMedia media;
                        media.id = pj2Str(cur_media->media.id);
                        media.type = pj2Str(cur_media->media.type);
                        media.src_id = pj2Str(cur_media->media.src_id);
                        media.status = cur_media->media.status;

                        endpoint.media.push_back(media);
                        cur_media = cur_media->next;
                    }
                    
                    user.endpoint.push_back(endpoint);
                    cur_endpoint = cur_endpoint->next;
                }

                conf_users.push_back(user);
                cur_user = cur_user->next;
            }

        }
#else
        connectedUserCount = pbi.conf_info.status.connected;
        totalUserCount     = pbi.conf_info.status.total;
        event               = pj2Str(pbi.conf_info.status.event);
        conferenceId        = pj2Str(pbi.conf_info.status.conferenceId);
        causedby            = pj2Str(pbi.conf_info.status.causedby);
#endif
    }
    else {
        infoUpdated = false;
#if 0 // nugucall - conference-info simple
        conf_users.clear();
#endif
    }
        
}

//////////////////////////////////////////////////////////////////////////////

/*
 * Constructor.
 */
Conference::Conference()
: id(PJSUA_INVALID_ID)
{
}
 
/*
 * Destructor.
 */
Conference::~Conference()
{
    if (isValid()) {
	pjsua_conference_set_user_data(id, NULL);
	pjsua_conference_del(id);

	/* Remove from account conference list */
	acc->removeConference(this);
    }
}
    
/*
 * Create conference and register the conference to PJSUA-LIB.
 */
void Conference::create(Account &account, const ConferenceConfig &cfg) throw(Error)
{
    pjsua_conference_config pj_cfg;
    pjsua_conference_config_default(&pj_cfg);
    
    if (!account.isValid())
	PJSUA2_RAISE_ERROR3(PJ_EINVAL, "Conference::create()", "Invalid account");
    
    pj_cfg.uri = str2Pj(cfg.uri);
    pj_cfg.subscribe = cfg.subscribe;
    pj_cfg.user_data = (void*)this;
    PJSUA2_CHECK_EXPR( pjsua_conference_add(&pj_cfg, &id) );
    
    acc = &account;
    acc->addConference(this);
}
    
/*
 * Check if this conference is valid.
 */
bool Conference::isValid() const
{
    return PJ2BOOL( pjsua_conference_is_valid(id) );
}

/*
 * Get detailed conference info.
 */
ConferenceInfo Conference::getInfo() const throw(Error)
{
    pjsua_conference_info pj_bi;
    ConferenceInfo bi;

    PJSUA2_CHECK_EXPR( pjsua_conference_get_info(id, &pj_bi) );
    bi.fromPj(pj_bi);
    return bi;
}

/*
 * Enable/disable conference's conference monitoring.
 */
void Conference::subscribeConference(bool subscribe) throw(Error)
{
    PJSUA2_CHECK_EXPR( pjsua_conference_subscribe_conf(id, subscribe) );
}

    
/*
 * Update the conference information for the conference.
 */
void Conference::updateConference(void) throw(Error)
{
    PJSUA2_CHECK_EXPR( pjsua_conference_update_conf(id) );
}
     
// /*
//  * Send instant messaging outside dialog.
//  */
// void Conference::sendInstantMessage(const SendInstantMessageParam &prm) throw(Error)
// {
//     ConferenceInfo bi = getInfo();

//     pj_str_t to = str2Pj(bi.contact.empty()? bi.uri : bi.contact);
//     pj_str_t mime_type = str2Pj(prm.contentType);
//     pj_str_t content = str2Pj(prm.content);
//     void *user_data = (void*)prm.userData;
//     pjsua_msg_data msg_data;
//     prm.txOption.toPj(msg_data);
    
//     PJSUA2_CHECK_EXPR( pjsua_im_send(acc->getId(), &to, &mime_type, &content,
// 				     &msg_data, user_data) );
// }

// /*
//  * Send typing indication outside dialog.
//  */
// void Conference::sendTypingIndication(const SendTypingIndicationParam &prm)
//      throw(Error)
// {
//     ConferenceInfo bi = getInfo();

//     pj_str_t to = str2Pj(bi.contact.empty()? bi.uri : bi.contact);
//     pjsua_msg_data msg_data;
//     prm.txOption.toPj(msg_data);
    
//     PJSUA2_CHECK_EXPR( pjsua_im_typing(acc->getId(), &to, prm.isTyping,
// 				       &msg_data) );
// }

