# Sunlight Documentation Contributors Guide

### **Important: Make sure you have cloned the Sunlight repository before following these instructions.**

The Sunlight Documentation can be viewed [here](Sunlight.rtfd.io). The documentation is built using [sphinx](https://docs.readthedocs.io/en/stable/intro/getting-started-with-sphinx.html). These instructions assume that you have already cloned the Sunlight repository. I would recommend working in Linux or Windows Subsystem for Linux (WSL).

## Tools setup instructions (Linux/WSL)

1. Open up a terminal.
2. Make sure Python and pip are installed. If they are not, you can install them by running `sudo apt install python3` and `sudo apt install pip3`.
3. Install all of the packages we are using for our docs by navigating to the `Sunlight/Docs` folder and running `pip3 install -r requirements.txt`.
4. To make sure everything is installed properly, run `make html` in the `Sunlight/Docs` folder. This will build the docs based on the code on your machine. You should get a message that ends in "The HTML pages are in build/html."

### Common Issues (Linux/WSL)
- If you need to add sphinx to your path, you can do it by:
    - Open your .bashrc file. You can do this by running `vim $HOME/.bashrc` or `nano $HOME/.bashrc` for example.
    - Scroll to the bottom and add the directory containing `sphinx-build`, `sphinx-quickstart`, etc to your path by adding
      the following line: `export PATH="$PATH:<your path>"`
    - For example, if the path you want to add is `$HOME/.local/bin` (I think this is the default on Linux/WSL), write
      `export PATH="$PATH:$HOME/.local/bin"`
    - Save and exit the file
    - Launch a new instance of your shell (run `bash`)

## Tools setup instructions (Windows)

1. Install WSL/Linux
2. Follow "Tools setup instructions (Linux/WSL)"

## Editing the docs
5. To open the built docs, navigate to `Sunlight/Docs/build/html` and open the file called `index.html` in a web browser (This can be done in your terminal or in a file explorer).
6. To edit the content of the docs, edit the `.rst` files in `Sunlight/Docs/source` with a text editor of choice. If you do not have one, I recommend [VSCode](https://code.visualstudio.com/download). The documentation is written in [markdown](https://www.markdownguide.org/basic-syntax/).
7. If you want to see your changes, run `make html` in the `Sunlight/Docs` folder and it will rebuild the docs.


For any questions, post in the BPS Slack channel or contact the BPS lead (Manthan Upadhyaya)
