import time
import random
import requests
import DAN
import paho.mqtt.client as mqtt

ServerURL = 'http://120.108.111.234:9999'  # with non-secure connection
# ServerURL = 'https://DomainName' #with SSL connection
Reg_addr = None  # if None, Reg_addr = MAC address

DAN.profile['dm_name'] = '108021061' #新建iottalk裝置名稱
DAN.profile['df_list'] = ['Humidity, Temperature, light108021061']    #新建iottalk裝置Device_Feature名稱，,humi108021061,light108021061
DAN.profile['d_name'] = '108021061'  #這個裝置的名稱

DAN.device_registration_with_retry(ServerURL, Reg_addr)
# DAN.deregister()  #if you want to deregister this device, uncomment this line
# exit()            #if you want to deregister this device, uncomment this line

DAN.ControlChannel
det_Temperature= 0
det_Humidity = 0
light108021061 = 0
control_message = False

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # 將訂閱主題寫在on_connet中
    # 如果我們失去連線或重新連線時
    # 地端程式將會重新訂閱
 
    client.subscribe("108022062") #MQTT的Topic訂閱

# 當接收到從伺服器發送的訊息時要進行的動作

def on_message(client, userdata, msg):
    global det_Temperature,det_Humidity, control_message

    # 轉換編碼utf-8才看得懂中文

    message = msg.payload.decode('utf-8')
    print(msg.topic,message, type(message))

    #這個部分為以溫溼度為例子切出字串

    detect_data = message.split(",")
    
    det_Temperature=float(detect_data[0])
    det_Humidity =float(detect_data[1])
    light108021061 =float(detect_data[1])

    
    detect_data = message.split(",")
    control_message = True
    print("iottalk",det_Temperature,det_Humidity,light108021061)

# 連線設定
# 初始化地端程式
client = mqtt.Client()

# 設定連線的動作
client.on_connect = on_connect

# 設定接收訊息的動作
client.on_message = on_message
print("client:", client.on_message)

# 設定登入帳號密碼
client.username_pw_set("test", "test")

# 設定連線資訊(IP, Port, 連線時間)
client.connect("120.108.111.227", 1883, 60)

# 開始連線，執行設定的動作和處理重新連線問題
# 也可以手動使用其他loop函式來進行連接

while True:
    if DAN.state == "RESUME":
        try:
            client.loop_start()
            if (control_message == True):
                DAN.push('Humidity', det_Humidity )
                DAN.push('Temperature', det_Temperature)
                DAN.push('light108021061', light108021061)

                control_message = False
            # ==================================

            # # Pull data from an output device feature "Dummy_Control"
            # ODF_data = DAN.pull('Dummy_Control')
            # if ODF_data != None:
            #     print(ODF_data[0])

        except Exception as e:
            print(e)
            if str(e).find('mac_addr not found:') != -1:
                print('Reg_addr is not found. Try to re-register...')
                DAN.device_registration_with_retry(ServerURL, Reg_addr)
            else:
                print('Connection failed due to unknow reasons.')
                time.sleep(1)
    else:
        client.loop_stop()
