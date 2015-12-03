#!/usr/bin/python


# import gui library
try:
    import wx
    import serial
except ImportError:
    raise ImportError,"The wxPython module and serial is required to run this program."


class select_effect(wx.Frame):	# inherit from base class for gui windows
	
	# initialize class
	def __init__(self, parent, id, title):
		wx.Frame.__init__(self, parent, id, title=title, size=(300, 300))	# call wk.Frame constructor
		self.parent = parent	# useful to keep track of parent (usually a parent container)
		self.init_elements()


	# initialize/create all the gui widgets needed for the app
	def init_elements(self):

		self.selected_effect = 0
		self.last_effect = 0
		self.output = [0, 0, 0, 0]

		self.panel = wx.Panel(self)     

		# set up sizers ---------------------------------------------------------------------
		# set sizer for the frame so we can resize window according to widgets/elements
		self.windowsizer = wx.BoxSizer()
		self.windowsizer.Add(self.panel, 1, wx.ALL | wx.EXPAND)        

		# set sizer for panel/grid layout for elements
		self.sizer = wx.GridBagSizer(5, 5)	# for effect

		# set sizer for a nice border
		self.border = wx.BoxSizer()
		self.border.Add(self.sizer, 1, wx.ALL | wx.EXPAND, 5)


		self.select_effect_label = wx.StaticText(self.panel, label="Select your GAPE effect, bro!")
		self.sizer.Add(self.select_effect_label, pos=(1, 1))


		# layout the 3 effect selector buttons ----------------------------------------------
		self.delay_button = wx.Button(self.panel, label="Delay", id=1)
		# on click call click function and pass the sizer to it to add more windows for the appropriate effect
		self.Bind(wx.EVT_BUTTON, lambda event, arg=1 : self.effect(arg), self.delay_button)
		# add the button to the layout
		self.sizer.Add(self.delay_button, pos=(2, 1))

		self.comp_button = wx.Button(self.panel, label="Compressor", id=2)
		# on click call click function and pass the sizer to it to add more windows for the appropriate effect
		self.Bind(wx.EVT_BUTTON, lambda event, arg=2 : self.effect(arg), self.comp_button)
		# add button to the layout
		self.sizer.Add(self.comp_button, pos=(4, 1))

		self.eq_button = wx.Button(self.panel, label="Equalizer", id=3)
		# on click call click function and pass the sizer to it to add more windows for the appropriate effect
		self.Bind(wx.EVT_BUTTON, lambda event, arg=3 : self.effect(arg), self.eq_button)
		# add the button to the layout
		self.sizer.Add(self.eq_button, pos=(6, 1))


		# then tell app to use the sizers
		self.panel.SetSizerAndFit(self.border)
		self.SetSizerAndFit(self.windowsizer)


	# event handler on click of effect button
	def effect(self, arg):

		# delay
		if arg == 1:
			# for knowing what effect was selected to build output string for dsp code
			self.selected_effect = arg
			# show delay params
			self.on_delay_click()	# checks last_effect from last effect selection to destroy the appropriate widgets
			# now set last effect to delay for the refresh function
			self.last_effect = arg

		# compressor
		elif arg == 2:
			# for knowing what effect was selected to build output string for dsp code
			self.selected_effect = arg
			# show compressor params
			self.on_comp_click()	# checks last_effect from last effect selection to destroy the appropriate widgets
			# now set last effect to comp for refresh function
			self.last_effect = arg

		# equalizer
		elif arg == 3: 
			# for knowing what effect was selected to build output string for dsp code
			self.selected_effect = arg
			# show eq params
			self.on_eq_click()	# checks last_effect from last effect selection to destroy the appropriate widgets
			# now set last effect to eq for refresh function
			self.last_effect = arg


		# now that effect and parameters are chosen, make a submit button
		self.enter_button = wx.Button(self.panel, label="Gape-ify me, Captain! (Submit)")
		# call function to deal with the input data (eventually send to stm board)
		self.Bind(wx.EVT_BUTTON, self.on_submit_effect, self.enter_button)
		# add button to layout
		self.sizer.Add(self.enter_button, pos=(9, 3))
		self.Layout()


	def refresh(self):

		if self.last_effect == 1:
			# clear delay fields ----------------
			self.time_input_label.Destroy()
			self.time_input.Destroy()
			self.gain_input_label.Destroy()
			self.gain_input.Destroy()

		if self.last_effect == 2:
			# clear compressor fields ----------------
			self.threshold_input_label.Destroy()
			self.threshold_input.Destroy()
			self.ratio_input_label.Destroy()
			self.ratio_input.Destroy()

		if self.last_effect == 3:
		# clear eq fields ----------------
			self.low_input_label.Destroy()
			self.low_input.Destroy()
			self.mid_input_label.Destroy()
			self.mid_input.Destroy()
			self.high_input_label.Destroy()
			self.high_input.Destroy()

		# if the submit button exists, destroy it. it exists if the last effect wasn't the default value of 0
		if self.last_effect != 0:
			self.enter_button.Destroy()



	# event handlers for selecting the effect -----------------------------------------------
	# this function is called if the delay effect is selected
	def on_delay_click(self):
		# delete any parameter fields if they exist so we can redraw the new ones
		self.refresh()

		# enter amount of delay --------------------------
		# enter time label
		self.time_input_label = wx.StaticText(self.panel, label="Enter amount of delay in seconds (max is 0.5, min is 0)")
		self.sizer.Add(self.time_input_label, pos=(1, 5))
		# enter time field
		self.time_input = wx.TextCtrl(self.panel, value="0.5", size=(100, -1))
		self.sizer.Add(self.time_input, pos=(2, 5))

		# enter gain of the delayed signal ---------------
		# enter gain label
		self.gain_input_label = wx.StaticText(self.panel, label="Enter the gain of the delayed signal (max is 1, min is 0)")
		self.sizer.Add(self.gain_input_label, pos=(3, 5))
		# enter gain field
		self.gain_input = wx.TextCtrl(self.panel, value="1", size=(100, -1))
		self.sizer.Add(self.gain_input, pos=(4, 5))

		# fix the layout of widgets
		self.Layout()

	def on_comp_click(self):
		# delete any parameter fields if they exist so we can redraw the new ones
		self.refresh()

		# enter threshold level --------------------------
		# enter threshold label
		self.threshold_input_label = wx.StaticText(self.panel, label="Enter the threshold level when the comp will kick in (max is 6dB)")
		self.sizer.Add(self.threshold_input_label, pos=(1, 5))
		# enter threshold field
		self.threshold_input = wx.TextCtrl(self.panel, value="-7", size=(100, -1))
		self.sizer.Add(self.threshold_input, pos=(2, 5))

		# enter ratio of the compressor ---------------
		# enter ratio label
		self.ratio_input_label = wx.StaticText(self.panel, label="Enter the ratio to compress the signal by (min is 1)")
		self.sizer.Add(self.ratio_input_label, pos=(3, 5))
		# enter ratio field
		self.ratio_input = wx.TextCtrl(self.panel, value="5", size=(100, -1))
		self.sizer.Add(self.ratio_input, pos=(4, 5))

		# fix the layout of widgets
		self.Layout()

	def on_eq_click(self):
		# delete any parameter fields if they exist so we can redraw the new ones
		self.refresh()

		# enter low band gain/attenuation ----------------
		# enter low band label
		self.low_input_label = wx.StaticText(self.panel, label="Enter the low band gain (-10dB <= gain <= 10dB)")
		self.sizer.Add(self.low_input_label, pos=(1, 5))
		# enter low band field
		self.low_input = wx.TextCtrl(self.panel, value="0", size=(100, -1))
		self.sizer.Add(self.low_input, pos=(2, 5))

		# enter mid band gain/attenuation ----------------
		# enter mid band label
		self.mid_input_label = wx.StaticText(self.panel, label="Enter the mid band gain (-10dB <= gain <= 10dB)")
		self.sizer.Add(self.mid_input_label, pos=(3, 5))
		# enter mid band field
		self.mid_input = wx.TextCtrl(self.panel, value="0", size=(100, -1))
		self.sizer.Add(self.mid_input, pos=(4, 5))

		# enter high band gain/attenuation ----------------
		# enter high band label
		self.high_input_label = wx.StaticText(self.panel, label="Enter the high band gain (-10dB <= gain <= 10dB)")
		self.sizer.Add(self.high_input_label, pos=(5, 5))
		# enter mid band field
		self.high_input = wx.TextCtrl(self.panel, value="0", size=(100, -1))
		self.sizer.Add(self.high_input, pos=(6, 5))

		# fix the layout of widgets
		self.Layout()


	# called when value is outside of appropriate bounds
	def error(self):
		dlg = wx.MessageDialog(self, 'Please make sure to enter parameter within the appropriate bounds.', 'Value Error', wx.OK|wx.ICON_INFORMATION)
		dlg.ShowModal()
		dlg.Destroy()



	def on_submit_effect(self, event):

		# get output array for delay effect
		if self.selected_effect == 1:
			# put delay effect as selected effect in the output buffer
			self.output[0] = 1
			# amount of delay in seconds
			self.delay_time = float(self.time_input.GetValue())
			if self.delay_time < 0 or self.delay_time > 0.5:
				self.error()
				return
			else: 
				# highest amount of delay is 0.5 (255), put in output buffer
				self.output[1] = int(self.delay_time * (2.0 * 255.0))

			# gain of delayed signal
			self.delay_gain = float(self.gain_input.GetValue())
			if self.delay_gain > 1 or self.delay_gain < 0:
				self.error()
				return
			else:
				# highest amount of gain is 1 (255), put in output buffer
				self.output[2] = int(self.delay_gain * 255.0)

		elif self.selected_effect == 2:
			# put compressor as selected effect in the output buffer
			self.output[0] = 2
			# threshold val
			self.threshold = int(self.threshold_input.GetValue())
			if self.threshold > 6:
				self.error()
				return
			else:
				# adding 200 will make sure there is no neg val, put in output buffer
				self.output[1] = self.threshold + 200

			# ratio val
			self.ratio = int(self.ratio_input.GetValue())
			if self.ratio < 1 or self.ratio > 254:
				self.error()
				return
			else:
				# put ratio in output buffer, no mapping needed
				self.output[2] = self.ratio

		elif self.selected_effect == 3:
			# put eq as selected effect in the output buffer
			self.output[0] = 3

			# band gain vals
			self.low = int(self.low_input.GetValue())
			if self.low > 10 or self.low < -10:
				self.error()
				return
			else:
				self.output[1] = self.low + 10

			self.mid = int(self.mid_input.GetValue())
			if self.mid > 10 or self.mid < -10:
				self.error()
				return
			else:
				self.output[2] = self.mid + 10

			self.high = int(self.high_input.GetValue())
			if self.high > 10 or self.high < -10:
				self.error()
				return
			else:
				self.output[3] = self.high + 10


		print self.output
		# ser = serial.Serial('/dev/ttyUSB0', 9600, serial.EIGHTBITS, serial.PARITY_NONE, serial.STOPBITS_ONE)
		# ser.write(self.output)




# main()
if __name__ == "__main__":

	# instance of app
	app = wx.App()
    # instance of select_effect
    # (no parent, -1 to let wx choose an identifier, window title, size of gui window)
	frame = select_effect(None, -1, 'Select Your GAPE Effect')
	frame.Show()
    # while(1) to wait and handle events such as click of button, quitting, etc.
	app.MainLoop()