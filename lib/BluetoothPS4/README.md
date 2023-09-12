# BluetoothPS4

HIDインターフェースクラスを継承したBluetooth接続のPS4コントローラ受信ライブラリ

---

## トラブルシューティング

- ### PS4コントローラがペアリングに失敗する

    原因として以下が考えられる

    - PS4に設定した Bluetooth MAC アドレスがが間違っている

        ESP32が個別で持っているMACアドレスをPS4コントローラに書き込む必要がある

        `SixaxisPairTool`を使って書き込むこと

    - ESP32内に過去のコントローラMACが残っている

        ESP32内に接続したコントローラのMACアドレスを確認する機能がある。
        この記憶をリセットしないと接続できない

        ```bash
        pip install esptool
        esptoo.py --chip esp32 erase_flash
        ```