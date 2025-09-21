# PhoneRemoteToPC
## Introduction
  <p>(OPS! this is a personal project for my own consumption, this is only for a learning experience and is not supposed to be seen as a commercial project)</p>
  <p>This project are two different application, "PCRemoteServer" as the server at the PC built with C++ and visual studio, and "AndroidUnityClient" which is for a android client built on Unity version 2023.2.20f1.</p>
  <p>This project allows you to connect your phone to your PC as a remote to execute bat scripts from the script folder, or code defined in PCRemoteServer/uh/Functions.h, while also having background images/videos. </p>
<div>
  <div>Client (background not included)</div>
  <div float="left">
    <img src="https://github.com/Jason-Diesel/PhoneRemoteToPC_/blob/main/ImagesForReadMe/Client1.PNG" width="40%">
    <img src="https://github.com/Jason-Diesel/PhoneRemoteToPC_/blob/main/ImagesForReadMe/Client2.PNG" width="40%">
  </div>
  <div>Server</div>
  <div float="left">
    <img src="https://github.com/Jason-Diesel/PhoneRemoteToPC_/blob/main/ImagesForReadMe/ServerScreen1.PNG" width="40%">
    <img src="https://github.com/Jason-Diesel/PhoneRemoteToPC_/blob/main/ImagesForReadMe/ServerScreen2.PNG" width="40%">
  </div>
</div>

## Usage
<p>
  Create a script by going into PCRemoteServer/Scripts/ and create a bat script there, (The program get the Scripts by using the path "../Scripts"),
  or you can also create a function in C++ by going into PCRemoteServer/uh/Functions.h and defining your own function.<br>
  These can later be activated by starting the server, start the client, write the code on the client given by the server (OPS both server and client must be on the same local network (bcs nobody wanna do shit like portforward(and I don't wanna create my own global server for this shit))), and then pressing the corresponding button 
</p>

## 
