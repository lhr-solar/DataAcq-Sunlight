**********************************
Miscellaneous Software Information
**********************************

Global Configuration
====================

Sunlight's global configuration is in ``config.h`` (`Github <https://github.com/lhr-solar/Sunlight/blob/main/Core/Inc/config.h>`_). 

Configuration options include: 

* **Debugging Mode** - enable/disable debugging statements
* **CAN Loopback** - enable/disable CAN loopback (*if loopback is enabled, debugging mode MUST be on*)
* **Heartbeat Period** - period in OS ticks (currently 1 OS tick = 1 ms)
* **Client/Server IPv4** - IPv4 address/netmask/gateway/port

Debugging Print
===============

A ``debugprintf()`` macro is included in ``config.h``. Calling ``debugprintf()`` 
uses the same syntax as ``printf()``.

``debugprintf()`` prints messages to UART, with the file name and line number automatically 
prepended to all messages. Messages can be globally disabled with the **Debugging Mode** option.
