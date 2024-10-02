Arduino powinno pracować na porcie COM3, fragment kodu z kodu na backendzie gdzie Node.js łączy się po serial-porcie z Arduino : 
Ustawienia portu można ustawić w panelu sterowania w zakładce menadżer urządzeń.


 path: 'COM3',
    baudRate: 9600,
    dataBits: 8,
    parity: 'none',
    stopBits: 1,
    flowControl: false
