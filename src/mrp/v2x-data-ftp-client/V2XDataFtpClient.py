"""
***************************************************************************************

 © 2019 Arizona Board of Regents on behalf of the University of Arizona with rights
       granted for USDOT OSADP distribution with the Apache 2.0 open source license.

***************************************************************************************

V2XDataFtpClient.py
Created by: Niraj Vasant Altekar
University of Arizona   
College of Engineering

This code was developed under the supervision of Professor Larry Head
in the Systems and Industrial Engineering Department.

***************************************************************************************
"""

import os
from ftplib import FTP

class V2XDataFtpClient:
    def __init__(self, ftpServerIp: str, ftpServerPort:int, clientUsername:str, clientPassword:str):
        """
        Establishes an FTP connection with the server and logs in as a client
        """
        self.ftpServerIp = ftpServerIp

        self.ftp = FTP()
        self.ftp.connect(ftpServerIp, port=ftpServerPort)
        self.ftp.login(user=clientUsername, passwd=clientPassword)

    def transfer_data(self, directoryPath:str, filename:str):
        """
        Transfers the all data from the directoryPath to appropriate locations in the root directory allocated by the server
        """
        if "spat" in filename: storCommand = "STOR spat/" + filename
        elif "remoteBsm" in filename: storCommand = "STOR remoteBsm/" + filename
        elif "srm" in filename: storCommand = "STOR srm/" + filename
        elif "ssm" in filename: storCommand = "STOR ssm/" + filename
        elif "msgCount" in filename: storCommand = "STOR msgCount/" + filename
        else: return

        try:
            with open((directoryPath + "/" + filename),'rb') as file:
                self.ftp.storbinary(storCommand, file)
            os.remove((directoryPath + "/" + filename))
        except FTP.all_errors:
            pass
    
    def close_connection(self):
        """
        Logs out from the established connection and closes the connection
        """
        self.ftp.quit()