# Sunlight
This Repo Contains all of Sunlight's code. Sunlight is Gary's telemetry system.

## Setup
Development is done in a Linux environment to build and flash the Sunlight code.

### Cloning the Repository
The most common way to clone repos is to use the HTTPS link, but the most secure way is to clone with SSH. The submodules use SSH, so you must generate and add an SSH key to your GitHub account. Follow the instructions in this tutorial. If this is your first time doing this, you can start at the [Generating a new SSH key and adding it to the ssh-agent](https://docs.github.com/en/github/authenticating-to-github/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent) step, but reading everything is beneficial.

Once you're able to use Git with SSH, go to the green "Code" button of the repo, and click "Use SSH" to get the URL. Copy the `git@github.com:repo-name` link. Then, enter the following command to the terminal but with the correct URL:

```
git clone  git@github.com:repo-name
```

### Setup for Terminal Development
Sunlight is designed to be built and deployed from a Linux terminal, allowing you to use your choice of text editor/IDE.
1. Ensure that you have some type of Linux machine such as Ubuntu, Windows Subsystem for Linux, etc. Windows will not work.
    - While you can compile the code on WSL, you will not be able to flash the code unless it is through a Linux machine (VM, dual boot, etc.)
2. The Sunlight code supports multiple microcontrollers and different architectures. Depending on which on you're using, make sure you have the correct toolchain.
    - STM32F413: Follow the instructions in this [repo](https://github.com/SijWoo/ARM-Toolchain-Setup) to setup the toolchains for the current telemetry board.
    - Simulator: Install GCC and GDB using the following lines
        ```
        sudo apt install gcc gdb
        ```
3. (Optional) Download [VSCode](https://code.visualstudio.com/)
4. (Optional) In VSCode, download the following extensions (Click on the block icon on the left column):
    - C/C++
    - ARM
    - cortex-debug
5. OPTIONAL: If you are testing on the simulator, you must add the following line in `/etc/security/limits.conf` then restart your machine:
    ```
    <username> - rtprio unlimited
    ```

## Building
When calling any of the following commands, make sure you are in the top level of the directory.

Call `make all` to compile the code. This is also the default command which will run if you call `make`.

Call `make clean` to remove all object files.

## Flashing
When calling any of the following commands, make sure you are in the top most level of the directory.
You must be on a Linux VM or dual boot machine. Consider using [VMWARE](https://kb.vmware.com/s/article/2057907) 
using this free license [key](https://gist.github.com/williamgh2019/cc2ad94cc18cb930a0aab42ed8d39e6f)

1. Install `st-util`
2. Connect your computer to the BPS using a Nucleo programmer
3. Call `make flash` to flash the most recently built BPS code. You may need to run this with `sudo`.

## Testing
The following testing information is specifically for terminal development.

### Using GDB
NOTE: The below is one way to debug the program. GDB is not that compatible with debugging the code so another method of debugging is also available.
GDB is a debugger program that can be used to step through a program as it is being run on the board. To use, you need two terminals open (if running a VM, you can SSH into your VM from your host machine with the command ssh <login>@<ip>), as well as a USB connection to the ST-Link programmer (as if you were going to flash the program to the board). 
1. In one terminal, run ```st-util```. This will launch a program that will interface with the board. 
2. In the other terminal, start gdb with the command ```gdb-multiarch ./build/Sunlight.elf``` (assuming that you are doing this in the root of the project directory.
3. This will launch GDB and read in all of the symbols from the program that you are running on the board. In order to actually connect gdb to the board, exectue the command ```target extended-remote localhost:4242```, which will connect to the st-util command from earlier.

### Using OPENOCD
OpenOCD is another debugger program that is open source and compatible with the STM32F413. 
1. Run ```st-util``` in one terminal.
2. Replace step 2 above with ```./openocd-debug.sh```.
3. Replace step 3 above with ```target extended-remote localhost:3333```

**Note:** If you get an error message for Permission denied, try giving openocd read/write permissions using chmod: ```chmod 764 openocd```
https://linuxcommand.org/lc3_lts0090.php

## Radio Setup
* Sunlight has a static IP of (192.168.1.221)
* Computer plugged into Endpoint (192.168.1.222)
* Sunlight plugged into Gateway (192.168.1.223)
* Set computer to a static IP of (192.168.1.224)
* All using port 65432.

* Config of Radio 884-5105:
    - IP:192.168.1.222
    - Network Type: Point-To-Point
    - Modem Mode: Endpoint
    - Call Book: 1->EndPoint Serial Number->8845003

* Config of Radio 884-5003:
    - IP:192.168.1.223
    - Network Type: Point-To-Point
    - Modem Mode: Gateway
    - Call Book: 1->EndPoint Serial Number->8845105

Both match the settings below:
![image](https://user-images.githubusercontent.com/89665539/216782825-0ffa80d5-61da-48d3-9d89-15e158f3f728.png)

## Rules
* Make sure to have fun!
* Commit frequently into your own branches. 
* Create a Pull Request whenever you are ready to add you working code to the `main` branch. 
* Make sure that your code passes all status checks before opening a pull request. 
* If you modify the code enough that the docs become out of date, update the docs in your pull request.
* You must select 1 reviewer for approval. The reviewers will make sure the code follows best practices.

### Code Reviews
You can review a pull request even if you are not an approver. In addition to helping us catch bugs, code reviews are a good opportunity to learn more about the BPS codebase. Some examples on what you can include in a code review are:
- Ask why the author wrote a certain part of code in a way they did
- Point out potential bugs in the code
- Point out that part of the code is not well documented or hard to read

You can either approve, comment, or request changes at the end of your pull request. These are the scenarios in which I would use each option:
- **Approve:** You are signing off that this pull request is ready to merge. You and the pull request author "own" this part of the code now.
- **Request Changes:** You have found problems with the code that must be addressed before it can be merged. Note that this will block the pull request from merging until you re-review the code.
- **Comment:** Do this if you do not want to explicitly approve or request changes.

Approvers:
1. Manthan Upadhyaya
2. Tianda Huang
3. Sidharth Babu
