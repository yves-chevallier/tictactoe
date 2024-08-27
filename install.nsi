OutFile "TicTacToeInstaller.exe"
InstallDir $PROGRAMFILES\TicTacToe
Page directory
Page instfiles

Section
  SetOutPath $INSTDIR
  File "C:\path\to\tictactoe.exe"
  CreateShortcut "$DESKTOP\TicTacToe.lnk" "$INSTDIR\tictactoe.exe"
SectionEnd
