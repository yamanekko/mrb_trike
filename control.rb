# control task
# ステアリング制御を行う

# coding: utf-8
LIGHT_WHITE = 20	# 白色の光センサ値
LIGHT_BLACK = 0		# 黒色の光センサ値

MAX_STEERING_ANGLE = 630
# DRIVING_POWER = 30	# サンプルコードの設定値
DRIVING_POWER = 25
# FRONT_POWER =  100	# サンプルコードの設定値
FRONT_POWER = 200

def log(logger, msg)
	value = logger.write(msg)
	if !value
		EV3RT::LCD.print("b log error!!")
	else
		EV3RT::LCD.print("b log ok")
	end
end


#for log
logger = EV3RT::Serial.new(EV3RT::SIO_PORT_BT)

led = EV3RT::LED.new()

colorSensor = EV3RT::ColorSensor.new(EV3RT::PORT_3)

leftMotor = EV3RT::Motor.new(EV3RT::PORT_A, EV3RT::LARGE_MOTOR)
rightMotor = EV3RT::Motor.new(EV3RT::PORT_B, EV3RT::LARGE_MOTOR)
frontMotor = EV3RT::Motor.new(EV3RT::PORT_C, EV3RT::LARGE_MOTOR)

clock = EV3RT::Clock.new()	# for log

#走行モータエンコーダリセット
leftMotor.reset
rightMotor.reset
frontMotor.reset

#走行可能(LEDは緑)
led.on(EV3RT::LED_GREEN)

# 初期処理をしたらいったんお休み
EV3RT::Task.sleep


loop do
  color = colorSensor.brightness	# log用に一度変数に入れる
  count = frontMotor.count
  if color >= (LIGHT_WHITE + LIGHT_BLACK)/2
   # 左旋回命令
	if count < MAX_STEERING_ANGLE
		frontMotor.pwm = FRONT_POWER
		pwm_front = FRONT_POWER	# for log
	else
		frontMotor.stop
		pwm_front = 0	# for log
	end
	leftMotor.pwm = -DRIVING_POWER
	rightMotor.pwm = 1
	pwm_left = -DRIVING_POWER	# for log
	pwm_right = 1	# for log
  else
    # 右旋回命令
	if count > -MAX_STEERING_ANGLE
		frontMotor.pwm = -FRONT_POWER
		pwm_front = -FRONT_POWER	# for log
	else
		frontMotor.stop
		pwm_front = 0	# for log
	end
	leftMotor.pwm = 1
	rightMotor.pwm = -DRIVING_POWER
	pwm_left = 1	# for log
	pwm_right = -DRIVING_POWER	# for log
  end

	msg = "time:#{clock.now},c:#{color},fc:#{count},fpwm:#{pwm_front},lpwm:#{pwm_left},rpwm:#{pwm_right}\r\n"
	log(logger, msg)

	EV3RT::Task.sleep

end #loop


