import requests
from time import sleep
import Adafruit_DHT as dht

#Enter Your API key here
APIKey="HIX6M6K9I73YIAJU"
#ThinSpeak API URL
URL='https://api.thingspeak.com/update?api_key=%s' % APIKey

while True:
        try:
                #Reading temperature and humidity from DHT22 in Raspberry Pi
                #In this setup, DHT22 connected to GPIO23
                humidity, temperature=dht.read_retry(dht.DHT22,23)
                print(humidity,temperature)
                #Send sensor data to ThingSpeak
                connection=requests.post(URL+'&field1=%.2f&field2=%.2f'%(temperature,humidity))

                sleep(10)
        except:
                print('Error occured')
                break
