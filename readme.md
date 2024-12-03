# Inti Marionette   



![alt text](https://github.com/georgesipp/IntiMarionette/blob/main/images/axis_remap_puppet.png)

Interface Puppe „Inti“ - BNO055 Sensoren
Im BNO055 ist ein Gyroskop, ein Beschleunigungssensor und ein Kompass verbaut. Auf dem Chip wird aus den Werten aller drei Sensoren die Rotation berechnet. Die Rotation wird oft in Eulerschen Winkeln angegeben, also drei Drehung von 0° – 360° entlang der X, Y und Z-Achse. Diese Drehungen werden auch mit Nicken, Rollen und Gieren (pitch, roll, yaw) beschrieben. Die Reihenfolge, in der die Drehung angewendet werden, ist wichtig, um gleiche Rotationen zu erhalten. 
![alt text](https://github.com/georgesipp/IntiMarionette/blob/main/images/llustrates-the-principle-of-gimbal-lock-The-outer-blue-frame-represents-the-x-axis-the.png)
Die Verwendung von Eulerschen Winkeln eignet sich für die Inti-Marionette aus verschiedenen Gründen nicht. Zum einen kann es bei Eulerschen Winkel zu einem Gimbal Lock (Abb. 1) kommen (Euler (gimbal lock) Explained). Dabei liegen zwei Achsen direkt übereinander und verhindern damit eine Rotationsmöglichkeit. Der Gimbal Lock führt dann in der Animation zu sprunghaften Bewegungen. Zum anderen stellt der BNO055 den Euler Winkel für den Pitch in Werten von -180° – 180° und den Roll in -90°¬ – 90° dar (Abb. 2). Die Werte eignen sich also nicht für die Weiterverarbeitung in Blender/Unity. 
Der BNO055 kann seine Rotation auch als Quaternion ausgeben. Eine Quaternion beschreibt die Rotation entlang einer bestimmen Achse und wird mit W, X, Y, Z dargestellt. Die Werte der Quaternion lassen sich nicht intuitiv interpretieren (How to think about Quaternions without your brain exploding). Anders als bei den Eulerschen Winkeln, steht die Anwendung der Rotationen fest. Daher können die Werte der Achsen nicht einfach neu zugeordnet werden, sondern die Achsen des Sensors selbst müssen neu konfiguriert werden.
 

Rotation
Die Achsen des BNO055 lassen sich mit den Registern AXIS_MAP_CONFIG und AXIS_MAP_SIGN neu zuordnen. Die Standard-Achseneinstellung ist auf dem BNO055 verzeichnet (Abb. 3). Wenn der Sensor waagerecht und auf der Y-Achse Richtung Norden ausgerichtet ist, zeigt der Sensor auf den absoluten Nullpunkt. 
Bei der Inti-Marionette sind die Sensoren auf der Rückseite angebracht, die Achsen müssen also neu zugeordnet werden.
 Die Blume zeigt wo für die Sensoren oben sein soll. Die Rückseite der Sensoren soll jetzt in Richtung des Nullpunkts zeigen (Abb. 4).
 
Die Achsen können jetzt einfach getauscht werden, sodass sie sich wieder am ursprünglichen Koordinatensystem orientieren. Die X-Achse wird zur Z-Achse, die Z-Achse wird zur Y-Achse, die Y-Achse wird zur X-Achse (Abb. 5). Das BNO055 Datenblatt zeigt, wie die Achsen neu konfiguriert werden können (Abb. 6). Für unsere Konfiguration bedeutet das folgende Bit-Konfiguration:
Z	Y	X
00	10	01

Das AXIS_MAP_CONFIG Register kann nur als Hexadezimalzahl beschrieben werden. Die Bitabfolge 001001 wird hexadezimal als 0x09 geschrieben. 
 
Der Eintrag ist in der Adafruit Arduino Library für den BNO055 nicht vorhanden und muss manuell hinzugefügt werden. Mit einem Texteditor kann der Eintrag in der Adafruit_BNO055.h vorgenommen werden (Abb. 7).
Bei der Neukonfigurierung der Achsen muss ein rechtshändiges Koordinatensystem beibehalten werden. Mit der neuen Ausrichtung ist das nicht der Fall, daher müssen zwei Achsenvorzeichen geändert werden.  
Abbildung 5 zeigt, dass die Z-Achse und die Y-Achse gespiegelt werden müssen. Für diese Achsen ergibt sich die Bitabfolge 011 oder 0x03 in hexadezimal (Abb. 8). Der Eintrag ist in der Adafruit Library als REMAP_SIGN_P4 vorhanden.
 
Mit Hilfe der folgenden Funktionen aus der Adafruit Library können die Achsen jetzt entsprechend konfiguriert werden:
bno.setAxisRemap(Adafruit_BNO055::REMAP_CONFIG_PUPPET_BODY); bno.setAxisSign(Adafruit_BNO055::REMAP_SIGN_P4);
Die Achsen sind jetzt so eingestellt, dass die Rückseite der Sensoren in Richtung des absoluten Nullpunktes zeigen (Abb. 9).

