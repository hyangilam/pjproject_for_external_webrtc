/* $Id: conference.hpp 5672 2017-10-06 08:14:31Z riza $ */
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
#ifndef __PJSUA2_CONFERENCE_HPP__
#define __PJSUA2_CONFERENCE_HPP__


/**
 * @file pjsua2/conference.hpp
 * @brief PJSUA2 Conference Operations
 */
#include <pjsua2/persistent.hpp>
#include <pjsua2/siptypes.hpp>

/** PJSUA2 API is inside pj namespace */
namespace pj
{

/**
 * @defgroup PJSUA2_CONF Conference
 * @ingroup PJSUA2_Ref
 * @{
 */

using std::string;
using std::vector;


// /**
//  * This describes conference status.
//  */
// struct ConferenceStatus
// {
//     /**
//      * Conference's status.
//      */
//     pjsua_conference_status	 status;

//     // /**
//     //  * Text to describe conference's online status.
//     //  */
//     // string		 statusText;
    
//     // /**
//     //  * Activity type.
//     //  */
//     // pjrpid_activity	 activity;

//     // /**
//     //  * Optional text describing the person/element.
//     //  */
//     // string		 note;

//     // /**
//     //  * Optional RPID ID string.
//     //  */
//     // string		 rpidId;

// public:
//     /**
//      * Constructor.
//      */
//     ConferenceStatus();
// };


/**
 * This structure describes conference configuration when adding a conference to
 * the conference list with Conference::create().
 */
struct ConferenceConfig : public PersistentObject
{
    /**
     * Conference URL or name address.
     */
    string	 	 uri;

    /**
     * Specify whether conference subscription should start immediately.
     */
    bool	 	 subscribe;

public:
    /**
     * Read this object from a container node.
     *
     * @param node		Container to read values from.
     */
    virtual void readObject(const ContainerNode &node) throw(Error);

    /**
     * Write this object to a container node.
     *
     * @param node		Container to write values to.
     */
    virtual void writeObject(ContainerNode &node) const throw(Error);
};

struct ConferenceMedia{
    string id;
    string type;
    string src_id;
    pjsip_conf_media_status_type status;
};
struct ConferenceEndpoint{
    string entity;
    string display_text;
    pjsip_conf_endpoint_status_type status;
    std::vector<ConferenceMedia> media;
};

struct ConferenceUser{
    string entity;
    string display_text;
    std::vector<ConferenceEndpoint> endpoint;
};

/**
 * This structure describes conference info, which can be retrieved by via
 * Conference::getInfo().
 */
struct ConferenceInfo
{
    /**
     * The full URI of the conference, as specified in the configuration.
     */
    string		 uri;

    /**
     * Conference's Contact, only available when conference subscription has
     * been established to the conference.
     */
    string		 contact;

    /**
     * Flag to indicate that we should monitor the conference information for
     * this conference (normally yes, unless explicitly disabled).
     */
    bool		 confMonitorEnabled;

    /**
     * If \a confMonitorEnabled is true, this specifies the last state of
     * the conference subscription. If conference subscription session is currently
     * active, the value will be PJSIP_EVSUB_STATE_ACTIVE. If conference
     * subscription request has been rejected, the value will be
     * PJSIP_EVSUB_STATE_TERMINATED, and the termination reason will be
     * specified in \a subTermReason.
     */
    pjsip_evsub_state	 subState;

    /**
     * String representation of subscription state.
     */
    string	         subStateName;

    /**
     * Specifies the last conference subscription termination code. This would
     * return the last status of the SUBSCRIBE request. If the subscription
     * is terminated with NOTIFY by the server, this value will be set to
     * 200, and subscription termination reason will be given in the
     * \a subTermReason field.
     */
    pjsip_status_code	 subTermCode;

    /**
     * Specifies the last conference subscription termination reason. If 
     * conference subscription is currently active, the value will be empty.
     */
    string		 subTermReason;

    // /**
    //  * Conference status.
    //  */
    // ConferenceStatus	 confStatus;
    // const pjsip_conf_type*    pInfo;
    bool            infoUpdated;
    unsigned int    connectedUserCount;
    unsigned int    totalUserCount;
    string          event;
    string          conferenceId;
    string          causedby;

public:
    /** Import from pjsip structure */
    void fromPj(const pjsua_conference_info &pbi);
};


/**
 * This structure contains parameters for Conference::onConferenceEvSubState() callback.
 */
struct OnConferenceEvSubStateParam
{
    /**
     * * The event which triggers state change event.
     */
    SipEvent    e;
};


/**
 * Conference.
 */
class Conference
{
public:
    /**
     * Constructor.
     */
    Conference();
    
    /**
     * Destructor. Note that if the Conference instance is deleted, it will also
     * delete the corresponding conference in the PJSUA-LIB.
     */
    virtual ~Conference();
    
    /**
     * Create conference and register the conference to PJSUA-LIB.
     *
     * @param acc		The account for this conference.
     * @param cfg		The conference config.
     */
    void create(Account &acc, const ConferenceConfig &cfg) throw(Error);
    
    /**
     * Check if this conference is valid.
     *
     * @return			True if it is.
     */
    bool isValid() const;

    /**
     * Get detailed conference info.
     *
     * @return			Conference info.
     */
    ConferenceInfo getInfo() const throw(Error);

    /**
     * Enable/disable conference's conference monitoring. Once conference's conference is
     * subscribed, application will be informed about conference's conference status
     * changed via \a onConferenceState() callback.
     *
     * @param subscribe		Specify true to activate conference
     *				subscription.
     */
    void subscribeConference(bool subscribe) throw(Error);
    
    /**
     * Update the conference information for the conference. Although the library
     * periodically refreshes the conference subscription for all buddies,
     * some application may want to refresh the conference's conference subscription
     * immediately, and in this case it can use this function to accomplish
     * this.
     *
     * Note that the conference's conference subscription will only be initiated
     * if conference monitoring is enabled for the conference. See
     * subscribeConference() for more info. Also if conference subscription for
     * the conference is already active, this function will not do anything.
     *
     * Once the conference subscription is activated successfully for the conference,
     * application will be notified about the conference's conference status in the
     * \a onConferenceState() callback.
     */
     void updateConference(void) throw(Error);
     
    // /**
    //  * Send instant messaging outside dialog, using this conference's specified
    //  * account for route set and authentication.
    //  *
    //  * @param prm	Sending instant message parameter.
    //  */
    // void sendInstantMessage(const SendInstantMessageParam &prm) throw(Error);

    // /**
    //  * Send typing indication outside dialog.
    //  *
    //  * @param prm	Sending instant message parameter.
    //  */
    // void sendTypingIndication(const SendTypingIndicationParam &prm)
	//  throw(Error);

public:
    /*
     * Callbacks
     */
     
    /**
     * Notify application when the conference state has changed.
     * Application may then query the conference info to get the details.
     */
    virtual void onConferenceState()
    {}

    /**
     * Notify application when the state of client subscription session
     * associated with a conference has changed. Application may use this
     * callback to retrieve more detailed information about the state
     * changed event.
     *
     * @param prm	Callback parameter.
     */
    virtual void onConferenceEvSubState(OnConferenceEvSubStateParam &prm)
    { PJ_UNUSED_ARG(prm); }
     
private:
     /**
      * Conference ID.
      */
     pjsua_conference_id	 id;
     
     /**
      * Account.
      */
     Account		*acc;
};


/** Array of buddies */
typedef std::vector<Conference*> ConferenceVector;


/**
 * @}  // PJSUA2_CONF
 */

} // namespace pj

#endif	/* __PJSUA2_CONFERENCE_HPP__ */
