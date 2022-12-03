# address = "F4:12:FA:44:14:C1"

import asyncio
from bleak import BleakClient
from struct import unpack
import csv
from sklearn import svm
import numpy as np
from sklearn.linear_model import LogisticRegression

address = "F4:12:FA:44:14:C1"
filename = "test.csv"

ACCEL_X_UUID = "b7ad0680-4aca-4b27-8f4b-8592a6e53d3e"
ACCEL_Y_UUID = "b7ad0681-4aca-4b27-8f4b-8592a6e53d3e"
ACCEL_Z_UUID = "b7ad0682-4aca-4b27-8f4b-8592a6e53d3e"
GYRO_X_UUID = "b7ad0683-4aca-4b27-8f4b-8592a6e53d3e"
GYRO_Y_UUID = "b7ad0684-4aca-4b27-8f4b-8592a6e53d3e"
GYRO_Z_UUID = "b7ad0685-4aca-4b27-8f4b-8592a6e53d3e"

async def main(address):
    async with BleakClient(address) as client:
        data = np.loadtxt("Datasets/simple_dataset.csv", delimiter = ',')

        x = data[:,:-1]
        y = data[:,-1]

        svm_model = svm.SVC()
        svm_model.fit(x, y)

        logostic_model = LogisticRegression().fit(x, y)

        with open(filename, 'w', newline='') as csvfile: 
            csvwriter = csv.writer(csvfile)
            while True:
                accel_x_raw = await client.read_gatt_char(ACCEL_X_UUID)
                accel_x = unpack('f', accel_x_raw)[0]
                accel_y_raw = await client.read_gatt_char(ACCEL_Y_UUID)
                accel_y = unpack('f', accel_y_raw)[0]
                accel_z_raw = await client.read_gatt_char(ACCEL_Z_UUID)
                accel_z = unpack('f', accel_z_raw)[0]
                gyro_x_raw = await client.read_gatt_char(GYRO_X_UUID)
                gyro_x = unpack('f', gyro_x_raw)[0]
                gyro_y_raw = await client.read_gatt_char(GYRO_Y_UUID)
                gyro_y = unpack('f', gyro_y_raw)[0]
                gyro_z_raw = await client.read_gatt_char(GYRO_Z_UUID)
                gyro_z = unpack('f', gyro_z_raw)[0]
                # print("Accel:", accel_x, accel_y, accel_z, "Gyro:", gyro_x, gyro_y, gyro_z)
                row = [accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z]
                x_pred = np.array(row).reshape(1, -1)

                svm_prediction = "na"
                log_prediction = "na"
                if (svm_model.predict(x_pred) == 1):
                    svm_prediction = "Biting    "
                else:
                    svm_prediction = "Not Biting"
                if (logostic_model.predict(x_pred) == 1):
                    log_prediction = "Biting    "
                else:
                    log_prediction = "Not Biting"
                print("SVM:", svm_prediction, "| Logistic:", log_prediction)
                csvwriter.writerow(row)

asyncio.run(main(address))