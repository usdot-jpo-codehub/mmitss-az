"""
***************************************************************************************

 © 2019 Arizona Board of Regents on behalf of the University of Arizona with rights
       granted for USDOT OSADP distribution with the Apache 2.0 open source license.

***************************************************************************************

v2x-data-cyverse-interface.py
Created by: Niraj Vasant Altekar
University of Arizona   
College of Engineering

This code was developed under the supervision of Professor Larry Head
in the Systems and Industrial Engineering Department.

***************************************************************************************
"""

import os
import json
from pyftpdlib.authorizers import DummyAuthorizer
from pyftpdlib.handlers import FTPHandler
from pyftpdlib.servers import FTPServer

DEFAULT_CLIENT_PASSWORD = "MmitssIntersection"
MAX_CONNECTIONS_TOTAL = 256
MAX_CONNECTIONS_PER_IP = 5

def main():
    # Read the configuration file
    with open("/nojournal/bin/v2x-data-ftp-server-config.json") as configFile:
        config = json.load(configFile)

    clients = config["Clients"]

    # Instantiate a dummy authorizer for managing 'virtual' users
    authorizer = DummyAuthorizer()

    for client in clients:
        add_client(client["Name"], client["LocalDirectory"], authorizer)

    # Instantiate FTP handler class
    handler = FTPHandler
    handler.authorizer = authorizer

    # Instantiate FTP server class to listen at specified address
    address = (config["FtpHost"], config["FtpPort"])
    server = FTPServer(address, handler)

    # set a limit for connections
    server.max_cons = MAX_CONNECTIONS_TOTAL
    server.max_cons_per_ip = MAX_CONNECTIONS_PER_IP

    # start ftp server and serve it forever
    server.serve_forever()

def add_client(name:str, localDirectory:str, authorizer:DummyAuthorizer):
    create_client_directory(localDirectory, "spat")
    create_client_directory(localDirectory, "remoteBsm")
    create_client_directory(localDirectory, "srm")
    create_client_directory(localDirectory, "ssm")
    create_client_directory(localDirectory, "msgCount")

    # Add clients as authorised users:
    authorizer.add_user(username=name,
                        password=DEFAULT_CLIENT_PASSWORD,
                        homedir=localDirectory, 
                        perm='elradfmwMT')

def create_client_directory(localDirectory:str, dataType:str):
    if not os.path.exists((localDirectory + "/" + dataType)):
        os.makedirs((localDirectory + "/" + dataType))

if __name__ == "__main__":
    main()

