Real-time C3D (RTC3D) protocol implementation

The goal: seamlessly integrate third party applications with Optotrak measurement system

The approach: Let our application called NDI First Principles take care of the system setup up to the point of tracking. Stream all real-time
	         3D, 6D and analog data to the client for the visualization/post-processing purposes. Both applications can be ran on the same computer
	         but 2 different computers can be used as well since the communication between server and client happens over TCP/IP protocol.

What is included in the third party developer's package:
1. 	Get the newest version of NDI First Principles application from NDI support site. It allows users to setup 
	the system and collect the data on any Optotrak 3020 or Optotrak Certus systems or playback previously collected data.
	
	You can use this application as the RTC3D server for your plugin development efforts. 
	If you do not have an access to Optotrak equipment you can feed the server the data from the .nco collection file which contains previuosly
	collected data. To be able to use server during playback, you will have to use the application in "Playback Only" mode. See Help->Licensing...
	and select "Activate Free Version (Playback Only)".

	Mastercollection.nco collection can be found in data. Select "Loop" at the bottom for continuous playback.
	
	As the default, the server is listening on port 3020. The port can be redefined in the c:/ndigital/settings/firstprinciples.ini file.
	In addition the server implements a simple http server which always replies with the current server parameters. You can check it out at
	http://first-principles-computer:3020

2.	Real Time C3D client containing the same version of the protocol as found in First Principles. This client comes with the source code.
	This client comes as is. Feel free to use it to speed up your plugin development. This client does't contain XML parser.

	Programmers Note: 
	The dll is built with the  Multithreaded DLL compiler option. If your application is using a different setting you may
	run into run-time errors caused by different memory heaps. You will run into problems if you let the client to allocate memory 
	( add 3d or tool items ) in the GetLatest3d( vector<Position3d> & vLatest3d ) member function and try to deallocate 
	it in your application using a different heap. The workaround is to make sure the vLatest3d vector contains enough items. You are free to use
	client sample code and use it in your plugin.

3.	test-client-server.exe application is a simple application adding the GUI to the RTC3D client. You can use it connect to the First Principles
	( or any other existing RT C3D server ) , issue RTC3D commands and see streaming marker and tool data. The application is using
	the same version of the client dll part of the package.
	At the moment the application connects only to the port 3020 on the server.


