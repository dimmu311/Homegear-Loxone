# Homegear-Loxone
Loxone Miniserver module for Homegear.<br />

this module connects Homegear to a Loxone Miniserver using the [Loxone Websocket API](https://www.loxone.com/enen/kb/api/) to connect to the Miniserver. 
To run this module you need Homegear.
For more information about how to use Homegear, visit [Homegear Website](https://homegear.eu) or [Homegear Github Repositorys](https://github.com/Homegear).

<h3>Installation</h3>
the homegear team kindly compiles this module on their compile server and makes it available as a debian packet.
after successful homegear installation, the Loxone module can easily be installed using this command.


```console
dimmu@homegearpi(rw):apt install homegear-loxone
```
now go down to configure the module

<h3>Build Your Own</h3>
If you don't want to install the prebuild version you can simply build your own version.
you should have running homegear version on your system.<br><br>
First install all the dependency


```console
dimmu@homegearpi(rw):apt install libgnutls28-dev libgnutlsxx28
```
Then compile and install


```console
dimmu@homegearpi(rw):/home/pi/Homegear-Loxone# ./makeRelease.sh
```
After compiling and installation is done you have to move and modify some files.


```console
dimmu@homegearpi(rw):/home/pi/Homegear-Loxone# cp 'misc/Config Directory/loxone.conf' /etc/homegear/families/loxone.conf
dimmu@homegearpi(rw):/home/pi/Homegear-Loxone# mkdir /etc/homegear/devices/65/
dimmu@homegearpi(rw):/home/pi/Homegear-Loxone# cp 'misc/Device Description Files/*' /etc/homegear/devices/65/
```

<h3>Configure The module</h3>
to configure the module you have to modify the config file. See the command below.
<br>
most of the config file is self-explanatory.
the only thing i want to remind is to set the Communication Module Header. And this header has to be between [ ].

*i recommend to generate extra username and password for Homegear on your Loxone Miniserver. By this way you don't need to chang anything in this configuration after updating users/passwords that you use on the Loxone App. Also you can set lower access permission for the Homegear user in the Miniserver. The Homegear user only needs permission for lokal connection, no FTP or something else.*


```console
dimmu@homegearpi(rw):/home/pi/Homegear-Loxone# nano /etc/homegear/families/loxone.conf
````


<h3>Working With The Module</h3>
The main part clould be done using homegear cli. i also testet some functions im Homegear Admin ui. The most important thinks work also in Admin ui, but not all. Because of that i only show how to work with cli. <br>
log in to homegear cli and select Loxone family (family nr. 65) module and show the module help.


```console
dimmu@homegearpi(rw):homegear -r
> fs 65
Family 65> help
List of commands (shortcut in brackets):

For more information about the individual command type: COMMAND help
peers list (ls)         List all peers
peers remove (prm)      Remove a peer (without unpairing)
peers setname (pn)      Name a peer
search (sp)             Searches for new devices
unselect (u)            Unselect this device
````

the most important command are:
 - peers list (ls) to show the existing peers
 - search (sp) to search vor new peers.
 
so at first commissioning just do "sp". The Output show you how much new peers was added.
```console
Family 65> sp
Search completed. Found 27 new peers.
````
the next you can show all your peers by typing "ls"
```console
 ID │ Name                      │ Room                      │      Serial Number │ Type │ Type String               │ Unreach
────┼───────────────────────────┼───────────────────────────┼────────────────────┼──────┼───────────────────────────┼────────
 28 │ Light Controller          │ Wohnzimmer                │ 16453104-032f-4c.. │ 0504 │ Loxone Central Light C... │      No
 29 │ Light Zentral             │ Wohnzimmer                │ 1645321b-035b-a7.. │ 0501 │ Loxone Central Audio Zone │      No
````
The show ID is the peer id.<br>
Name and Room is what you set in Loxone Config. Please see the Limitations if you are wondering about the names.<br>

If you get really long list of peers, you can do filter during ls.
See this example to filter for all Peers in a Room
```console
Family 65> ls room Wohnzimmer
````
by this way you can easily find a peer id that you maybe need for nood-blue programming.

<h3>Limitations</h3>
until now there are some issues:
*    there is no function to check if the connection was possible. Till now the only way is to look in the log during homegear restart.
*    not all Loxone Devices have Device Descriptions until now. But i think the most important are there. Devices without Device Description fils did not create a Homegear peer.
*    there some things Loxone changes in the Structfile after you upload now Project with Loxone Config. When you do search peers, this module gets new sturctfile. if there are new Devices in the stuctfile you get new Homegear peer. But if there is a change to a already know Device (in this case there is already a peer) the existing peer is not updatet. Eg if you change Room or Category of a Device in Loxone Config, Homegear do not change the Roomname or the Categorie.
*    if you remove Device in Loxone Config that is already know to Homegear, this peer do not show that it do not exist anymore.

If you find any more issu (i'm sure there are) feel free to open issu or ask in homegear forum.
