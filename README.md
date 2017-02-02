# wificredentialsviewer

www.wificredentials.com
Based on an article of Michael Haephrati at Code Project.
https://www.codeproject.com/Articles/1164749/The-Secrets-of-Wi-Fi-Credentials
All graphics are copyrighted to Secured Globe, Inc. New York www.securedglobe.com

`GetWifiPasswords` lists stored WLAN profiles (and older Protected Storage keys) and writes `SG_WiFiPasswordList.txt`. Build `GetWifiData.sln`.

## Unit tests

`Tests/WifiTests` checks production `pstore.h` key and confirmation flags, the WLAN plaintext-profile flag used in `GetWifiPasswords.cpp`, console color constants, and the same `%hX` hex dump as `StringToHexString`. Build **Release | Win32** and run `WifiTests.exe`.
