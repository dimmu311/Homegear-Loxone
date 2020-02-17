# Homegear-Loxone
Loxone Miniserver module for Homegear.<br />

this module connects Homegear to a Loxone Miniserver by using the [Loxone Websocket API](https://www.loxone.com/enen/kb/api/) to connect to the Miniserver. 
To run this module you need Homegear.
For more information about how to use Homegear, visit [Homegear Website](https://homegear.eu) or [Homegear Github Repositorys](https://github.com/Homegear).

<h3>Installation</h3>
until now there is no prebuild version of this module to download. Maybe, when the module is more grown there would be prebuild version to download.
<br/>
Anyway, it is pretty easy to compile your on version. You sholud already have setup Homegear. Then, simply do the following steps do download, compile and install the module on your system.

```console
ToDo
```
After compiling and installation is done you have to move and modify some Files.
```console
dimmu@homegearpi(rw):/home/pi/Homegear-Loxone# cp 'misc/Config Directory/loxone.conf' /etc/homegear/families/loxone.conf
dimmu@homegearpi(rw):/home/pi/Homegear-Loxone# mkdir /etc/homegear/devices/65/
dimmu@homegearpi(rw):/home/pi/Homegear-Loxone# cp 'misc/Device Description Files/*' /etc/homegear/devices/65/
```
The next step is to configure the module
```console
dimmu@homegearpi(rw):/home/pi/Homegear-Loxone# nano /etc/homegear/families/loxone.conf
````
i recomend to generate extra username and passwort for Homegear on your Loxone Miniserver. By this way you don't need to chang anything in this configuratan after updating users/passworts that you use on the Loxone App. Also you can set lower access permisson for the Homegear user. The Homegear user only needs permison for lokal connection, no FTP or something else.
