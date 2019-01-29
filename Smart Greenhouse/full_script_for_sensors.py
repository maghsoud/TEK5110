import requests # library for HTTP requests
from time import sleep
import Adafruit_DHT as dht # library for DHT22 sensor
import Adafruit_ADS1x15 # library for soil moisture sensor
from twilio.rest import Client #library for sending SMS
import collections

# stuff for thingSpeak
APIKey="R2GTGW0UJZJSIESJ"
URL='https://api.thingspeak.com/update?api_key=%s'%APIKey

# stuff for Kibana
URL1='https://project1.basicinternet.org'

# stuff for Twilio
account_sid = 'AC15f3074e44f564118a7df456a0e23ac9'
auth_token = '38b4ef4f6c6e8404a413ac7c3016b840'
client = Client(account_sid, auth_token)

# soil moisture sensor setup
# Choose a gain of 1 for reading voltages from 0 to 4.09V.
# Or pick a different gain to change the range of voltages that are read:
#  - 2/3 = +/-6.144V
#  -   1 = +/-4.096V
#  -   2 = +/-2.048V
#  -   4 = +/-1.024V
#  -   8 = +/-0.512V
#  -  16 = +/-0.256V
# See table 3 in the ADS1015/ADS1115 datasheet for more info on gain.
# maximum digital output of ADC is 32767
# Define ADC input line and the gain for Voltage sensor module
soilInput=0
soilGain=2/3 #when soil sensor connected to 5V

# Found voltage when soil sensor is 100% in water
soilSensor_maxVoltage=1.4571
# Found voltage when soil sensor is dry
soilSensor_DryVoltage=5.1948

# This section convert voltage read by  ADC to percentage (0-100%)
# for more information read Calculating Scale and Offset in  https://racelogic.support
# calculate voltage scale or gradient of line
# dy/dx=(100-0)/(soilSensor_maxVoltage-soilSensor_DryVoltage)
scale=100/(soilSensor_maxVoltage-soilSensor_DryVoltage)
# calculate ofsset: Y=mX+c -> percent=(scale*Voltage)+offset
offset=(-scale*soilSensor_DryVoltage)

while True:
	try:
        # Read data from DHT22 sensor
		humidity,temperature=dht.read_retry(dht.DHT22,23)
        # Read ADC converted value from Analog Input for Voltage
        soil_adc=adc.read_adc(soilInput,soilGain)
        # Calculate actual voltage from ADC data
        soilMoistVoltage=((soil_adc*6.144)/32767)
        # Calculate soil moisture percentage
        soilMoist_percentage=(scale*soilMoistVoltage)+offset
        print('| {moisture} | {soil}| {soilPercent:.2f}%|  '.format(moisture=soil_adc,soil=soilMoistVoltage,soilPercent=soilMoist_percentage))
        print(humidity,temperature)
		if temperature > 20.0 and humidity >= 10:
			message = client.messages \
				.create(
					body='Too hot in your greenhouse! Temperature = {temp:.2f}°C, Humidity = {hum:.2f}%'.format(temp=temperature, hum=humidity),
					from_='+4759446214',
					to='+33643269310'
				)
        if soilMoist_percentage > 150.0:
            message = client.messages \
                .create(
                    body='Your soil is too dry! Soil percent = {soilPerc:.2f}%'.format(soilPerc=soilMoist_percentage),
                    from_='+4759446214',
                    to='+33643269310'
                )

        # for sending to thingSpeak
        connection=requests.post(URL+'&field1=%.2f&field2=%.2f&field3=%.2f'%(temperature,humidity,soilMoist_percentage))
        # for sending to Kibana
        sensorData={'Humidity':humidity, 'Temperature':temperature, 'Voltage':soilMoist_percentage}
        connection=requests.post(URL1, data=sensorData, auth=('Project1','Tek5110'))
    
    sleep(5)
	except:
		print('Error occured')
		break
