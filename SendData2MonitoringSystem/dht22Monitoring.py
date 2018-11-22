
import requests
from time import sleep
import Adafruit_DHT as dht

#TEK5110 monitoring platform URL
URL='https://Your_project_URL' 

while True:
	try:
		#Reading temperature and humidity from DHT22 in Raspberry Pi
		#In this setup, DHT22 connected to GPIO23
		humidity, temperature=dht.read_retry(dht.DHT22,23)
		#Convert sensor measurements to dictionary format
		sensorData={'Humidity':humidity,'Temperature':temperature}
#		print(humidity,temperature)

		#Post sensor data to TEK5110 monitoring platform
		connection=requests.post(URL,data=sensorData, auth=('Your_username','Your_password'))
		
		sleep(10)
	except:
		print('Error occured')
		break

