/****************************************************************
 *  Vidalia is distributed under the following license:
 *
 *  Copyright (C) 2006,  Matt Edman, Justin Hipple
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 *  Boston, MA  02110-1301, USA.
 ****************************************************************/

/** 
 * \file torcontrol.h
 * \version $Id$
 * \brief Object for interacting with the Tor process and control interface
 */

#ifndef _TORCONTROL_H
#define _TORCONTROL_H

#include <QObject>
#include <QHash>
#include <QList>
#include <QStringList>

#include "controlconnection.h"
#include "torprocess.h"
#include "torevents.h"
#include "torsignal.h"
#include "routerdescriptor.h"
#include "addressmap.h"

#if defined(Q_OS_WIN32)
#include "torservice.h"
#endif


class TorControl : public QObject
{
  Q_OBJECT
  
public:
  /** Default constructor */
  TorControl();
  /** Default destructor */
  ~TorControl();

  /** Start the Tor process */
  void start();
  /** Stop the Tor process */
  bool stop(QString *errmsg = 0);
  /** Detect if the Tor process is running */
  bool isRunning();
  /** Detects if the Tor process is running under Vidalia. */
  bool isVidaliaRunningTor();
  
  /** Connect to Tor's control socket */
  void connect();
  /** Disconnect from Tor's control socket */
  void disconnect();
  /** Check if we're connected to Tor's control socket */
  bool isConnected();
  /** Sends an authentication token to Tor */
  bool authenticate(QString *errmsg = 0);

  /** Sends a GETINFO message to Tor based on the given keys */
  bool getInfo(QHash<QString,QString> &map, QString *errmsg = 0);
  /** Sends a GETINFO message for a single info value to Tor */
  bool getInfo(QString key, QString &val, QString *errmsg = 0);

  /** Sends a signal to Tor */
  bool signal(TorSignal::Signal sig, QString *errmsg = 0);
 
  /** Returns an address on which Tor is listening for application
   * requests. If none are available, a null QHostAddress is returned. */
  QHostAddress getSocksAddress(QString *errmsg = 0);
  /** Returns a (possibly empty) list of all currently configured 
   * SocksListenAddress entries. */
  QStringList getSocksAddressList(QString *errmsg = 0);
  /** Returns a valid SOCKS port for Tor, or 0 if Tor is not accepting
   * application requests. */
  quint16 getSocksPort(QString *errmsg = 0);
  /** Returns a list of all currently configured SOCKS ports. If Tor is not
   * accepting any application connections, an empty list will be returned. */
  QList<quint16> getSocksPortList(QString *errmsg = 0);

  /** Returns Tor's version as a string. */
  QString getTorVersionString();
  /** Returns Tor's version as a numeric value. */
  quint32 getTorVersion();

  /** Sets an event and its handler. If add is true, then the event is added,
   * otherwise it is removed. If set is true, then the given event will be
   * registered with Tor. */
  bool setEvent(TorEvents::TorEvent e, QObject *obj, 
                bool add, bool set = true, QString *errmsg = 0);
  /** Registers for a set of logging events according to the given filter. */
  bool setLogEvents(uint filter, QObject *obj, QString *errmsg = 0);
  /** Register events of interest with Tor */
  bool setEvents(QString *errmsg = 0);
  

  /** Sets each configuration key in <b>map</b> to the value associated with its key. */
  bool setConf(QHash<QString,QString> map, QString *errmsg = 0);
  /** Sets a single configuration key to the given value. */
  bool setConf(QString key, QString value, QString *errmsg = 0);
  /** Gets values for a set of configuration keys, each of which has a single
   * value. */
  bool getConf(QHash<QString,QString> &map, QString *errmsg = 0);
  /** Gets a set of configuration keyvalues and stores them in <b>map</b>. */
  bool getConf(QHash<QString,QStringList> &map, QString *errmsg = 0);
  /** Gets a single configuration value for <b>key</b>. */
  bool getConf(QString key, QString &value, QString *errmsg = 0);
  /** Gets a list of configuration values for <b>key</b>. */
  bool getConf(QString key, QStringList &value, QString *errmsg = 0);
  /** Asks Tor to save the current configuration to its torrc */
  bool saveConf(QString *errmsg = 0);
  /** Tells Tor to reset the given configuration keys back to defaults. */
  bool resetConf(QStringList keys, QString *errmsg = 0);
  /** Tells Tor to reset a configuration key back to its default value. */
  bool resetConf(QString key, QString *errmsg = 0);

  /** Gets a descriptor for the given router name. */
  RouterDescriptor getDescriptorByName(QString name, QString *errmsg = 0);
  /** Gets a descriptor for the given router ID. */
  RouterDescriptor getDescriptorById(QString id, QString *errmsg = 0);
  /** Gets descriptors for the given list of router names. */
  QList<RouterDescriptor> getDescriptorListByName(QStringList names, QString *errmsg = 0);
  /** Gets descriptors for the given list of router IDs. */
  QList<RouterDescriptor> getDescriptorListById(QStringList ids, QString *errmsg = 0);
  /** Gets a list of descriptors for all routers Tor knows about. */
  QList<RouterDescriptor> getRouterList(QString *errmsg = 0);
  /** Gets a list of router IDs for all routers Tor knows about. */
  QStringList getRouterIDList(QString *errmsg = 0);

  /** Gets a list of current circuits. */
  QList<Circuit> getCircuits(QString *errmsg = 0);
  /** Gets a list of current streams. */
  QList<Stream> getStreams(QString *errmsg = 0);
  
  /** Gets a list of address mappings of the type specified by <b>type</b>
   * (defaults to <i>AddressMapAll</i>. */
  AddressMap getAddressMap(
    AddressMap::AddressMapType type = AddressMap::AddressMapAll,
    QString *errmsg = 0);

public slots:
  /** Closes the circuit specified by <b>circid</b>. If <b>ifUnused</b> is
   * true, then the circuit will not be closed unless it is unused. */
  bool closeCircuit(quint64 circid, bool ifUnused = false, QString *errmsg = 0);
  /** Closes the stream specified by <b>streamid</b>. */
  bool closeStream(quint64 streamid, QString *errmsg = 0);

signals:
  /** Emitted when the Tor process has started */
  void started();
  /** Emitted when the Tor process fails to start. */
  void startFailed(QString errmsg);
  /** Emitted when the Tor process has stopped */
  void stopped(int exitCode, QProcess::ExitStatus exitStatus);
  /** Emitted when the Tor process has stopped. */
  void stopped();
  /** Emitted when the controller has connected to Tor */
  void connected();
  /** Emitted when the controller failed to connect to Tor. */
  void connectFailed(QString errmsg);
  /** Emitted when the controller has disconnected from Tor */
  void disconnected();
  /** Emitted when the connection status changes. */
  void connected(bool connected);

private:
  /** Instantiates a connection used to talk to Tor's control port */
  ControlConnection* _controlConn;
  /** Manages and monitors the Tor process */
  TorProcess* _torProcess;
  /** Keep track of which events we're interested in */
  TorEvents _torEvents;
  /** The version of Tor we're currently talking to. */
  QString _torVersion;
#if defined(Q_OS_WIN32)
  /** Manages the Tor service, if supported and enabled */
  TorService* _torService;
#endif

  /** Send a message to Tor and read the response */
  bool send(ControlCommand cmd, ControlReply &reply, QString *errmsg = 0);
  /** Send a message to Tor and discard the response */
  bool send(ControlCommand cmd, QString *errmsg = 0);
  /** Disconnects signals from the TorProcess and frees its memory. */
  void closeTorProcess();
  
/* The slots below simply relay signals from the appropriate member objects */
private slots:
  void onStarted();
  void onStartFailed(QString errmsg);
  void onStopped(int exitCode, QProcess::ExitStatus exitStatus);
  void onConnected();
  void onConnectFailed(QString errmsg);
  void onDisconnected();
  void onLogStdout(QString severity, QString message);
};

#endif

