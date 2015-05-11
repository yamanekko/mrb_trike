# coding: utf-8
LIGHT_WHITE = 40	# 白色の光センサ値
LIGHT_BLACK = 0		# 黒色の光センサ値

def log(logger, msg)
	value = logger.write(msg)
	if !value
		EV3RT::LCD.print("log error!!")
	else
		EV3RT::LCD.print("log ok")
	end
end

touchSensor = EV3RT::TouchSensor.new(EV3RT::PORT_2)

leftMotor = EV3RT::Motor.new(EV3RT::PORT_A, EV3RT::LARGE_MOTOR)
rightMotor = EV3RT::Motor.new(EV3RT::PORT_B, EV3RT::LARGE_MOTOR)
frontMotor = EV3RT::TailMotor.new(EV3RT::PORT_C, EV3RT::LARGE_MOTOR)
clock = EV3RT::Clock.new()

#for log
logger = EV3RT::Serial.new(EV3RT::SIO_PORT_BT)

# LCD 
led = EV3RT::LED.new()
led.on(EV3RT::LED_ORANGE)

# 初期処理だけして寝かす
EV3RT::Task.active(EV3RT::CONTROL_TASK_ID)

EV3RT::LCD.print("Trike sample mruby cyclick ver.")

# スタート待機(タッチセンサ押下を監視)
loop do

  if touchSensor.pressed?
    break
  end

  clock.sleep(10)

end	# loop end

EV3RT::Task.start_cyclic(EV3RT::CONTROL_CYC_ID)
EV3RT::LCD.print("sleep main task.")

# back ボタンで終了指示があるまでおやすみ
EV3RT::Task.sleep

# backボタンタスクより起動されたら終了処理を行う
frontMotor.stop
leftMotor.stop
rightMotor.stop

led.on(EV3RT::LED_RED)
