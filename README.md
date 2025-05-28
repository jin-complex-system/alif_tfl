# alif_tfl

## How to run the project with Visual Studio Code

1. Using Visual Studio Code, open the directory at [`alif_src`](alif_src)
2. Configure instructions according to README.md provided


# How to create a new project `preprocess`
1. Copy `blinky` project as it is and rename the directory to `preprocess`
2. Inside [tasks.json](/alif_src/.vscode/tasks.json), add the following at line 15:
```json
        {
            "label": "Clean all (including YML build files)",
            "type": "shell",
            "command": [
                "cbuild ${command:cmsis-csolution.getSolutionPath} --clean;",
                "rm -f ./blinky/*.cbuild.yml;",
                "rm -f ./hello/*.cbuild.yml;",
                "rm -f ./hello_rtt/*.cbuild.yml;",
                "rm -f ./preprocess/*.cbuild.yml;"
            ],
            "windows": {
                "command": [
                    "cbuild ${command:cmsis-csolution.getSolutionPath} --clean;",
                    "rm -Force ./blinky/*.cbuild.yml;",
                    "rm -Force ./hello/*.cbuild.yml;",
                    "rm -Force ./hello_rtt/*.cbuild.yml;",
                    "rm -Force ./preprocess/*.cbuild.yml;"
                ]
            },
            "problemMatcher": []
        },
```
3. Inside [alif.csolution.yml](/alif_src/alif.csolution.yml), add the following line at line 45:
```yml
  projects:
    - project: blinky/blinky.cproject.yml
    - project: hello/hello.cproject.yml
    - project: hello_rtt/hello_rtt.cproject.yml
    - project: preprocess/preprocess.cproject.yml
```
