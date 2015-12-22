#!/usr/bin/python


# import gui library
try:
	from subprocess import call 
	import wx
except ImportError:
	raise ImportError,"The wxPython module and subprocess is required to run this program."


class select_effect(wx.Frame):	# inherit from base class for gui windows
	""" Class with all the definitions to run the application to select the GAPE effect. """
	# initialize class
	def __init__(self, parent, id, title):
		wx.Frame.__init__(self, parent, id, title=title, size=(300, 300))	# call wk.Frame constructor
		self.Bind(wx.EVT_CLOSE, self.OnClose)
		self.parent = parent	# useful to keep track of parent (usually a parent container)
		self.init_elements()


	# initialize/create all the gui widgets needed for the app
	def init_elements(self):
		""" This sets up the windows and effect buttons. """
		self.selected_effect = 0
		self.selected_preset = 0
		self.count = 0
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


		self.select_effect_label = wx.StaticText(self.panel, label="Select your effect!")
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
		""" This handles the onclick event for when an effect button is selected. The appropriate function is called and the 
		current effect and last effect variables are set. This also adds the submit button to the frame for when the fields 
		are filled out. """
		# delay
		if arg == 1:
			# for knowing what effect was selected to build output string for dsp code
			self.selected_effect = arg
			# show delay params
			self.on_delay_click()	# checks last_effect from last effect selection to destroy the appropriate widgets and creates the new widgets for the selected effect
			# now set last effect to delay for the refresh function
			self.last_effect = arg

		# compressor
		elif arg == 2:
			# for knowing what effect was selected to build output string for dsp code
			self.selected_effect = arg
			# show compressor params
			self.on_comp_click()	# checks last_effect from last effect selection to destroy the appropriate widgetsand creates the new widgets for the selected effect
			# now set last effect to comp for refresh function
			self.last_effect = arg

		# equalizer
		elif arg == 3: 
			# for knowing what effect was selected to build output string for dsp code
			self.selected_effect = arg
			# show eq params
			self.on_eq_click()	# checks last_effect from last effect selection to destroy the appropriate widgetsand creates the new widgets for the selected effect
			# now set last effect to eq for refresh function
			self.last_effect = arg



		self.preset_label = wx.StaticText(self.panel, label="Preset")
		self.sizer.Add(self.preset_label, pos=(1, 4))

		# now that effect and parameters are chosen, make a submit button
		self.enter_button = wx.Button(self.panel, label="Submit")
		# call function to deal with the input data (eventually send to stm board)
		self.Bind(wx.EVT_BUTTON, self.on_submit_effect, self.enter_button)
		# add button to layout
		self.sizer.Add(self.enter_button, pos=(16, 4))
		self.Layout()


	def refresh(self):
		""" This destroys all the widgets created for the effect previously selected to make room for the current effect widgets. """
		if self.last_effect == 1:
			# clear delay fields ----------------
			self.preset1_button.Destroy()
			self.preset2_button.Destroy()
			self.time_input_label.Destroy()
			self.gain_input_label.Destroy()
			self.time_input1.Destroy()
			self.gain_input1.Destroy()
			self.time_input2.Destroy()
			self.gain_input2.Destroy()

		if self.last_effect == 2:
			# clear compressor fields ----------------
			self.preset3_button.Destroy()
			self.preset4_button.Destroy()
			self.threshold_input_label.Destroy()
			self.ratio_input_label.Destroy()
			self.threshold_input1.Destroy()
			self.ratio_input1.Destroy()
			self.threshold_input2.Destroy()
			self.ratio_input2.Destroy()

		if self.last_effect == 3:
		# clear eq fields ----------------
			self.preset5_button.Destroy()
			self.preset6_button.Destroy()
			self.preset7_button.Destroy()
			self.preset8_button.Destroy()
			self.preset9_button.Destroy()
			self.preset10_button.Destroy()
			self.preset11_button.Destroy()
			self.low_input_label.Destroy()
			self.mid_input_label.Destroy()
			self.high_input_label.Destroy()
			self.low_input1.Destroy()
			self.mid_input1.Destroy()
			self.high_input1.Destroy()
			self.low_input2.Destroy()
			self.mid_input2.Destroy()
			self.high_input2.Destroy()
			self.low_input3.Destroy()
			self.mid_input3.Destroy()
			self.high_input3.Destroy()
			self.low_input4.Destroy()
			self.mid_input4.Destroy()
			self.high_input4.Destroy()
			self.low_input5.Destroy()
			self.mid_input5.Destroy()
			self.high_input5.Destroy()
			self.low_input6.Destroy()
			self.mid_input6.Destroy()
			self.high_input6.Destroy()
			self.low_input7.Destroy()
			self.mid_input7.Destroy()
			self.high_input7.Destroy()


		# if the submit button exists, destroy it. it exists if the last effect wasn't the default value of 0
		if self.last_effect != 0:
			self.enter_button.Destroy()
			self.preset_label.Destroy()





	# event handlers for selecting the effect -----------------------------------------------
	def on_delay_click(self):
		""" This sets up the widgets for the delay parameters. """
		# delete any parameter fields if they exist so we can redraw the new ones
		self.refresh()

		# Labels --------------------------------------------------------
		# enter time label
		self.time_input_label = wx.StaticText(self.panel, label="Amount of delay (s)")
		self.sizer.Add(self.time_input_label, pos=(1, 6))

		# enter gain label
		self.gain_input_label = wx.StaticText(self.panel, label="Gain")
		self.sizer.Add(self.gain_input_label, pos=(1, 8))


		# PRESET 1 ------------------------------------------------------
		self.preset1_button = wx.Button(self.panel, label="Large Room")
		self.Bind(wx.EVT_BUTTON, self.on_large_room, self.preset1_button)
		self.sizer.Add(self.preset1_button, pos=(2, 4))
		# enter time field
		self.time_input1 = wx.TextCtrl(self.panel, value="0.5")
		self.time_input1.SetEditable(False)
		self.sizer.Add(self.time_input1, pos=(2, 6))
		# enter gain field
		self.gain_input1 = wx.TextCtrl(self.panel, value="0.5")
		self.gain_input1.SetEditable(False)
		self.sizer.Add(self.gain_input1, pos=(2, 8))

		# PRESET 2 ------------------------------------------------------
		self.preset2_button = wx.Button(self.panel, label="Small Room")
		self.Bind(wx.EVT_BUTTON, self.on_small_room, self.preset2_button)
		self.sizer.Add(self.preset2_button, pos=(4, 4))
		# enter time field
		self.time_input2 = wx.TextCtrl(self.panel, value="0.25")
		self.time_input2.SetEditable(False)
		self.sizer.Add(self.time_input2, pos=(4, 6))
		# enter gain field
		self.gain_input2 = wx.TextCtrl(self.panel, value="1")
		self.gain_input2.SetEditable(False)
		self.sizer.Add(self.gain_input2, pos=(4, 8))


		# fix the layout of widgets
		self.Layout()


	def on_comp_click(self):
		""" This sets up the widgets for the compressor parameters. """
		# delete any parameter fields if they exist so we can redraw the new ones
		self.refresh()

		# Labels ---------------------------------------------------
		self.threshold_input_label = wx.StaticText(self.panel, label="Threshold (dB)")
		self.sizer.Add(self.threshold_input_label, pos=(1, 6))

		self.ratio_input_label = wx.StaticText(self.panel, label="Ratio")
		self.sizer.Add(self.ratio_input_label, pos=(1, 8))

		# PRESET 3 - Coffee Shop -----------------------------------
		self.preset3_button = wx.Button(self.panel, label="Coffee Shop")
		self.Bind(wx.EVT_BUTTON, self.on_coffee_shop, self.preset3_button)
		self.sizer.Add(self.preset3_button, pos=(2, 4))
		# enter threshold field
		self.threshold_input1 = wx.TextCtrl(self.panel, value="-7")
		self.threshold_input1.SetEditable(False)
		self.sizer.Add(self.threshold_input1, pos=(2, 6))
		# enter ratio field
		self.ratio_input1 = wx.TextCtrl(self.panel, value="2")
		self.ratio_input1.SetEditable(False)
		self.sizer.Add(self.ratio_input1, pos=(2, 8))

		# PRESET 4 - Celestial Immolation --------------------------
		self.preset4_button = wx.Button(self.panel, label="Celestial Immolation")
		self.Bind(wx.EVT_BUTTON, self.on_celestial, self.preset4_button)
		self.sizer.Add(self.preset4_button, pos=(4, 4))
		# enter threshold field
		self.threshold_input2 = wx.TextCtrl(self.panel, value="-2")
		self.threshold_input2.SetEditable(False)
		self.sizer.Add(self.threshold_input2, pos=(4, 6))
		# enter ratio field
		self.ratio_input2 = wx.TextCtrl(self.panel, value="9")
		self.ratio_input2.SetEditable(False)
		self.sizer.Add(self.ratio_input2, pos=(4, 8))


		# fix the layout of widgets
		self.Layout()


	def on_eq_click(self):
		""" This sets up the widgets for the equalizer parameters. """
		# delete any parameter fields if they exist so we can redraw the new ones
		self.refresh()

		# Labels ---------------------------------------------------
		self.low_input_label = wx.StaticText(self.panel, label="Low Band")
		self.sizer.Add(self.low_input_label, pos=(1, 6))

		self.mid_input_label = wx.StaticText(self.panel, label="Mid Band")
		self.sizer.Add(self.mid_input_label, pos=(1, 8))

		self.high_input_label = wx.StaticText(self.panel, label="High Band")
		self.sizer.Add(self.high_input_label, pos=(1, 10))


		# PRESET 5 - Bass Boost ------------------------------------
		self.preset5_button = wx.Button(self.panel, label="Bass Boost")
		self.Bind(wx.EVT_BUTTON, self.on_bassboost, self.preset5_button)
		self.sizer.Add(self.preset5_button, pos=(2, 4))
		# enter low band field
		self.low_input1 = wx.TextCtrl(self.panel, value="10")
		self.low_input1.SetEditable(False)
		self.sizer.Add(self.low_input1, pos=(2, 6))
		# enter mid band field
		self.mid_input1 = wx.TextCtrl(self.panel, value="0")
		self.mid_input1.SetEditable(False)
		self.sizer.Add(self.mid_input1, pos=(2, 8))
		# enter mid band field
		self.high_input1 = wx.TextCtrl(self.panel, value="0")
		self.high_input1.SetEditable(False)
		self.sizer.Add(self.high_input1, pos=(2, 10))

		# PRESET 6 - Mid Boost -------------------------------------
		self.preset6_button = wx.Button(self.panel, label="Mid Boost")
		self.Bind(wx.EVT_BUTTON, self.on_midboost, self.preset6_button)
		self.sizer.Add(self.preset6_button, pos=(4, 4))
		# enter low band field
		self.low_input2 = wx.TextCtrl(self.panel, value="0")
		self.low_input2.SetEditable(False)
		self.sizer.Add(self.low_input2, pos=(4, 6))
		# enter mid band field
		self.mid_input2 = wx.TextCtrl(self.panel, value="10")
		self.mid_input2.SetEditable(False)
		self.sizer.Add(self.mid_input2, pos=(4, 8))
		# enter mid band field
		self.high_input2 = wx.TextCtrl(self.panel, value="0")
		self.high_input2.SetEditable(False)
		self.sizer.Add(self.high_input2, pos=(4, 10))

		# PRESET 7 - Treble Boost ------------------------------------
		self.preset7_button = wx.Button(self.panel, label="Treble Boost")
		self.Bind(wx.EVT_BUTTON, self.on_highboost, self.preset7_button)
		self.sizer.Add(self.preset7_button, pos=(6, 4))
		# enter low band field
		self.low_input3 = wx.TextCtrl(self.panel, value="0")
		self.low_input3.SetEditable(False)
		self.sizer.Add(self.low_input3, pos=(6, 6))
		# enter mid band field
		self.mid_input3 = wx.TextCtrl(self.panel, value="0")
		self.mid_input3.SetEditable(False)
		self.sizer.Add(self.mid_input3, pos=(6, 8))
		# enter mid band field
		self.high_input3 = wx.TextCtrl(self.panel, value="10")
		self.high_input3.SetEditable(False)
		self.sizer.Add(self.high_input3, pos=(6, 10))

		# PRESET 8 - Bass Attenuation ------------------------------------
		self.preset8_button = wx.Button(self.panel, label="Bass Attenuation")
		self.Bind(wx.EVT_BUTTON, self.on_basskill, self.preset8_button)
		self.sizer.Add(self.preset8_button, pos=(8, 4))
		# enter low band field
		self.low_input4 = wx.TextCtrl(self.panel, value="-10")
		self.low_input4.SetEditable(False)
		self.sizer.Add(self.low_input4, pos=(8, 6))
		# enter mid band field
		self.mid_input4 = wx.TextCtrl(self.panel, value="0")
		self.mid_input4.SetEditable(False)
		self.sizer.Add(self.mid_input4, pos=(8, 8))
		# enter mid band field
		self.high_input4 = wx.TextCtrl(self.panel, value="0")
		self.high_input4.SetEditable(False)
		self.sizer.Add(self.high_input4, pos=(8, 10))

		# PRESET 9 - Mid Attenuation -------------------------------------
		self.preset9_button = wx.Button(self.panel, label="Mid Attenuation")
		self.Bind(wx.EVT_BUTTON, self.on_midkill, self.preset9_button)
		self.sizer.Add(self.preset9_button, pos=(10, 4))
		# enter low band field
		self.low_input5 = wx.TextCtrl(self.panel, value="0")
		self.low_input5.SetEditable(False)
		self.sizer.Add(self.low_input5, pos=(10, 6))
		# enter mid band field
		self.mid_input5 = wx.TextCtrl(self.panel, value="-10")
		self.mid_input5.SetEditable(False)
		self.sizer.Add(self.mid_input5, pos=(10, 8))
		# enter mid band field
		self.high_input5 = wx.TextCtrl(self.panel, value="0")
		self.high_input5.SetEditable(False)
		self.sizer.Add(self.high_input5, pos=(10, 10))

		# PRESET 10 - Treble Attenuation ------------------------------------
		self.preset10_button = wx.Button(self.panel, label="Treble Attenuation")
		self.Bind(wx.EVT_BUTTON, self.on_highkill, self.preset10_button)
		self.sizer.Add(self.preset10_button, pos=(12, 4))
		# enter low band field
		self.low_input6 = wx.TextCtrl(self.panel, value="0")
		self.low_input6.SetEditable(False)
		self.sizer.Add(self.low_input6, pos=(12, 6))
		# enter mid band field
		self.mid_input6 = wx.TextCtrl(self.panel, value="0")
		self.mid_input6.SetEditable(False)
		self.sizer.Add(self.mid_input6, pos=(12, 8))
		# enter mid band field
		self.high_input6 = wx.TextCtrl(self.panel, value="-10")
		self.high_input6.SetEditable(False)
		self.sizer.Add(self.high_input6, pos=(12, 10))

		# PRESET 11 - Flat Response -----------------------------------------
		self.preset11_button = wx.Button(self.panel, label="Flat Response")
		self.Bind(wx.EVT_BUTTON, self.on_flat, self.preset11_button)
		self.sizer.Add(self.preset11_button, pos=(14, 4))
		# enter low band field
		self.low_input7 = wx.TextCtrl(self.panel, value="0")
		self.low_input7.SetEditable(False)
		self.sizer.Add(self.low_input7, pos=(14, 6))
		# enter mid band field
		self.mid_input7 = wx.TextCtrl(self.panel, value="0")
		self.mid_input7.SetEditable(False)
		self.sizer.Add(self.mid_input7, pos=(14, 8))
		# enter mid band field
		self.high_input7 = wx.TextCtrl(self.panel, value="0")
		self.high_input7.SetEditable(False)
		self.sizer.Add(self.high_input7, pos=(14, 10))


		# fix the layout of widgets
		self.Layout()



	# event handlers for on preset click -------------------------------------------------------


	# DELAY PRESETS ------------------------------------------------------
	# preset 1 - Large Room Delay
	def on_large_room(self, event):
		self.selected_preset = 1

		# highlight selected preset grey
		self.time_input1.SetBackgroundColour((111, 111, 111))
		self.gain_input1.SetBackgroundColour((111, 111, 111))
		self.time_input2.SetBackgroundColour((255, 255, 255))
		self.gain_input2.SetBackgroundColour((255, 255, 255))

	# preset 2 - Small Room Delay
	def on_small_room(self, event):
		self.selected_preset = 2

		# highlight selected preset grey
		self.time_input1.SetBackgroundColour((255, 255, 255))
		self.gain_input1.SetBackgroundColour((255, 255, 255))
		self.time_input2.SetBackgroundColour((111, 111, 111))
		self.gain_input2.SetBackgroundColour((111, 111, 111))


	# COMPRESSOR PRESETS -------------------------------------------------
	def on_coffee_shop(self, event):
		self.selected_preset = 3

		# highlight selected preset grey
		self.threshold_input1.SetBackgroundColour((111, 111, 111))
		self.ratio_input1.SetBackgroundColour((111, 111, 111))
		self.threshold_input2.SetBackgroundColour((255, 255, 255))
		self.ratio_input2.SetBackgroundColour((255, 255, 255))

	def on_celestial(self, event):
		self.selected_preset = 4

		# highlight selected preset grey
		self.threshold_input1.SetBackgroundColour((255, 255, 255))
		self.ratio_input1.SetBackgroundColour((255, 255, 255))
		self.threshold_input2.SetBackgroundColour((111, 111, 111))
		self.ratio_input2.SetBackgroundColour((111, 111, 111))


	# EQUALIZER PRESETS -------------------------------------------------
	def on_bassboost(self, event):
		self.selected_preset = 5

		# highlight selected preset grey
		self.low_input1.SetBackgroundColour((111, 111, 111))
		self.mid_input1.SetBackgroundColour((111, 111, 111))
		self.high_input1.SetBackgroundColour((111, 111, 111))
		self.low_input2.SetBackgroundColour((255, 255, 255))
		self.mid_input2.SetBackgroundColour((255, 255, 255))
		self.high_input2.SetBackgroundColour((255, 255, 255))
		self.low_input3.SetBackgroundColour((255, 255, 255))
		self.mid_input3.SetBackgroundColour((255, 255, 255))
		self.high_input3.SetBackgroundColour((255, 255, 255))
		self.low_input4.SetBackgroundColour((255, 255, 255))
		self.mid_input4.SetBackgroundColour((255, 255, 255))
		self.high_input4.SetBackgroundColour((255, 255, 255))
		self.low_input5.SetBackgroundColour((255, 255, 255))
		self.mid_input5.SetBackgroundColour((255, 255, 255))
		self.high_input5.SetBackgroundColour((255, 255, 255))
		self.low_input6.SetBackgroundColour((255, 255, 255))
		self.mid_input6.SetBackgroundColour((255, 255, 255))
		self.high_input6.SetBackgroundColour((255, 255, 255))
		self.low_input7.SetBackgroundColour((255, 255, 255))
		self.mid_input7.SetBackgroundColour((255, 255, 255))
		self.high_input7.SetBackgroundColour((255, 255, 255))

	def on_midboost(self, event):
		self.selected_preset = 6

		# highlight selected preset grey
		self.low_input1.SetBackgroundColour((255, 255, 255))
		self.mid_input1.SetBackgroundColour((255, 255, 255))
		self.high_input1.SetBackgroundColour((255, 255, 255))
		self.low_input2.SetBackgroundColour((111, 111, 111))
		self.mid_input2.SetBackgroundColour((111, 111, 111))
		self.high_input2.SetBackgroundColour((111, 111, 111))
		self.low_input3.SetBackgroundColour((255, 255, 255))
		self.mid_input3.SetBackgroundColour((255, 255, 255))
		self.high_input3.SetBackgroundColour((255, 255, 255))
		self.low_input4.SetBackgroundColour((255, 255, 255))
		self.mid_input4.SetBackgroundColour((255, 255, 255))
		self.high_input4.SetBackgroundColour((255, 255, 255))
		self.low_input5.SetBackgroundColour((255, 255, 255))
		self.mid_input5.SetBackgroundColour((255, 255, 255))
		self.high_input5.SetBackgroundColour((255, 255, 255))
		self.low_input6.SetBackgroundColour((255, 255, 255))
		self.mid_input6.SetBackgroundColour((255, 255, 255))
		self.high_input6.SetBackgroundColour((255, 255, 255))
		self.low_input7.SetBackgroundColour((255, 255, 255))
		self.mid_input7.SetBackgroundColour((255, 255, 255))
		self.high_input7.SetBackgroundColour((255, 255, 255))

	def on_highboost(self, event):
		self.selected_preset = 7

		# highlight selected preset grey
		self.low_input1.SetBackgroundColour((255, 255, 255))
		self.mid_input1.SetBackgroundColour((255, 255, 255))
		self.high_input1.SetBackgroundColour((255, 255, 255))
		self.low_input2.SetBackgroundColour((255, 255, 255))
		self.mid_input2.SetBackgroundColour((255, 255, 255))
		self.high_input2.SetBackgroundColour((255, 255, 255))
		self.low_input3.SetBackgroundColour((111, 111, 111))
		self.mid_input3.SetBackgroundColour((111, 111, 111))
		self.high_input3.SetBackgroundColour((111, 111, 111))
		self.low_input4.SetBackgroundColour((255, 255, 255))
		self.mid_input4.SetBackgroundColour((255, 255, 255))
		self.high_input4.SetBackgroundColour((255, 255, 255))
		self.low_input5.SetBackgroundColour((255, 255, 255))
		self.mid_input5.SetBackgroundColour((255, 255, 255))
		self.high_input5.SetBackgroundColour((255, 255, 255))
		self.low_input6.SetBackgroundColour((255, 255, 255))
		self.mid_input6.SetBackgroundColour((255, 255, 255))
		self.high_input6.SetBackgroundColour((255, 255, 255))
		self.low_input7.SetBackgroundColour((255, 255, 255))
		self.mid_input7.SetBackgroundColour((255, 255, 255))
		self.high_input7.SetBackgroundColour((255, 255, 255))


	def on_basskill(self, event):
		self.selected_preset = 8

		# highlight selected preset grey
		self.low_input1.SetBackgroundColour((255, 255, 255))
		self.mid_input1.SetBackgroundColour((255, 255, 255))
		self.high_input1.SetBackgroundColour((255, 255, 255))
		self.low_input2.SetBackgroundColour((255, 255, 255))
		self.mid_input2.SetBackgroundColour((255, 255, 255))
		self.high_input2.SetBackgroundColour((255, 255, 255))
		self.low_input3.SetBackgroundColour((255, 255, 255))
		self.mid_input3.SetBackgroundColour((255, 255, 255))
		self.high_input3.SetBackgroundColour((255, 255, 255))
		self.low_input4.SetBackgroundColour((111, 111, 111))
		self.mid_input4.SetBackgroundColour((111, 111, 111))
		self.high_input4.SetBackgroundColour((111, 111, 111))
		self.low_input5.SetBackgroundColour((255, 255, 255))
		self.mid_input5.SetBackgroundColour((255, 255, 255))
		self.high_input5.SetBackgroundColour((255, 255, 255))
		self.low_input6.SetBackgroundColour((255, 255, 255))
		self.mid_input6.SetBackgroundColour((255, 255, 255))
		self.high_input6.SetBackgroundColour((255, 255, 255))
		self.low_input7.SetBackgroundColour((255, 255, 255))
		self.mid_input7.SetBackgroundColour((255, 255, 255))
		self.high_input7.SetBackgroundColour((255, 255, 255))


	def on_midkill(self, event):
		self.selected_preset = 9

		# highlight selected preset grey
		self.low_input1.SetBackgroundColour((255, 255, 255))
		self.mid_input1.SetBackgroundColour((255, 255, 255))
		self.high_input1.SetBackgroundColour((255, 255, 255))
		self.low_input2.SetBackgroundColour((255, 255, 255))
		self.mid_input2.SetBackgroundColour((255, 255, 255))
		self.high_input2.SetBackgroundColour((255, 255, 255))
		self.low_input3.SetBackgroundColour((255, 255, 255))
		self.mid_input3.SetBackgroundColour((255, 255, 255))
		self.high_input3.SetBackgroundColour((255, 255, 255))
		self.low_input4.SetBackgroundColour((255, 255, 255))
		self.mid_input4.SetBackgroundColour((255, 255, 255))
		self.high_input4.SetBackgroundColour((255, 255, 255))
		self.low_input5.SetBackgroundColour((111, 111, 111))
		self.mid_input5.SetBackgroundColour((111, 111, 111))
		self.high_input5.SetBackgroundColour((111, 111, 111))
		self.low_input6.SetBackgroundColour((255, 255, 255))
		self.mid_input6.SetBackgroundColour((255, 255, 255))
		self.high_input6.SetBackgroundColour((255, 255, 255))
		self.low_input7.SetBackgroundColour((255, 255, 255))
		self.mid_input7.SetBackgroundColour((255, 255, 255))
		self.high_input7.SetBackgroundColour((255, 255, 255))


	def on_highkill(self, event):
		self.selected_preset = 10

		# highlight selected preset grey
		self.low_input1.SetBackgroundColour((255, 255, 255))
		self.mid_input1.SetBackgroundColour((255, 255, 255))
		self.high_input1.SetBackgroundColour((255, 255, 255))
		self.low_input2.SetBackgroundColour((255, 255, 255))
		self.mid_input2.SetBackgroundColour((255, 255, 255))
		self.high_input2.SetBackgroundColour((255, 255, 255))
		self.low_input3.SetBackgroundColour((255, 255, 255))
		self.mid_input3.SetBackgroundColour((255, 255, 255))
		self.high_input3.SetBackgroundColour((255, 255, 255))
		self.low_input4.SetBackgroundColour((255, 255, 255))
		self.mid_input4.SetBackgroundColour((255, 255, 255))
		self.high_input4.SetBackgroundColour((255, 255, 255))
		self.low_input5.SetBackgroundColour((255, 255, 255))
		self.mid_input5.SetBackgroundColour((255, 255, 255))
		self.high_input5.SetBackgroundColour((255, 255, 255))
		self.low_input6.SetBackgroundColour((111, 111, 111))
		self.mid_input6.SetBackgroundColour((111, 111, 111))
		self.high_input6.SetBackgroundColour((111, 111, 111))
		self.low_input7.SetBackgroundColour((255, 255, 255))
		self.mid_input7.SetBackgroundColour((255, 255, 255))
		self.high_input7.SetBackgroundColour((255, 255, 255))



	def on_flat(self, event):
		self.selected_preset = 11

		# highlight selected preset grey
		self.low_input1.SetBackgroundColour((255, 255, 255))
		self.mid_input1.SetBackgroundColour((255, 255, 255))
		self.high_input1.SetBackgroundColour((255, 255, 255))
		self.low_input2.SetBackgroundColour((255, 255, 255))
		self.mid_input2.SetBackgroundColour((255, 255, 255))
		self.high_input2.SetBackgroundColour((255, 255, 255))
		self.low_input3.SetBackgroundColour((255, 255, 255))
		self.mid_input3.SetBackgroundColour((255, 255, 255))
		self.high_input3.SetBackgroundColour((255, 255, 255))
		self.low_input4.SetBackgroundColour((255, 255, 255))
		self.mid_input4.SetBackgroundColour((255, 255, 255))
		self.high_input4.SetBackgroundColour((255, 255, 255))
		self.low_input5.SetBackgroundColour((255, 255, 255))
		self.mid_input5.SetBackgroundColour((255, 255, 255))
		self.high_input5.SetBackgroundColour((255, 255, 255))
		self.low_input6.SetBackgroundColour((255, 255, 255))
		self.mid_input6.SetBackgroundColour((255, 255, 255))
		self.high_input6.SetBackgroundColour((255, 255, 255))
		self.low_input7.SetBackgroundColour((111, 111, 111))
		self.mid_input7.SetBackgroundColour((111, 111, 111))
		self.high_input7.SetBackgroundColour((111, 111, 111))




	# called when value is outside of appropriate bounds
	def error(self):
		""" This is called whenever we receive any characters in the parameter inputs that we don't expect. """
		dlg = wx.MessageDialog(self, 'Please make sure to enter parameter within the appropriate bounds.', 'Value Error', wx.OK|wx.ICON_INFORMATION)
		dlg.ShowModal()
		dlg.Destroy()



	def on_submit_effect(self, event):
		""" This checks all the parameters from the input to make sure they are appropriate. If they are, they are set to
		to the output buffer. Then the output is sent to three different areas: the console, the display_effect program
		(7 seg display), and the dsp board through gpio. Note that the values are scaled in different ways so that we don't 
		need to deal with negative numbers. """

		# get output array for delay effect
		if self.selected_effect == 1:
			# put delay effect as selected effect in the output buffer
			self.output[0] = 1

			if self.selected_preset == 1:
				# amount of delay in seconds
				try:
					self.delay_time = float(self.time_input1.GetValue())
				except ValueError:
					self.error()
					return
				if self.delay_time < 0 or self.delay_time > 0.5:
					self.error()
					return
				else: 
					# highest amount of delay is 0.5 (255 for an 8bit value), put in output buffer
					self.output[1] = int(self.delay_time * (2.0 * 255.0))

				# gain of delayed signal
				try:
					self.delay_gain = float(self.gain_input1.GetValue())
				except ValueError:
					self.error()
					return
				if self.delay_gain > 1 or self.delay_gain < 0:
					self.error()
					return
				else:
					# highest amount of gain is 1 (255), put in output buffer
					self.output[2] = int(self.delay_gain * 255.0)

			elif self.selected_preset == 2:
				# amount of delay in seconds
				try:
					self.delay_time = float(self.time_input2.GetValue())
				except ValueError:
					self.error()
					return
				if self.delay_time < 0 or self.delay_time > 0.5:
					self.error()
					return
				else: 
					# highest amount of delay is 0.5 (255 for an 8bit value), put in output buffer
					self.output[1] = int(self.delay_time * (2.0 * 255.0))

				# gain of delayed signal
				try:
					self.delay_gain = float(self.gain_input2.GetValue())
				except ValueError:
					self.error()
					return
				if self.delay_gain > 1 or self.delay_gain < 0:
					self.error()
					return
				else:
					# highest amount of gain is 1 (255), put in output buffer
					self.output[2] = int(self.delay_gain * 255.0)



		elif self.selected_effect == 2:
			# put compressor as selected effect in the output buffer
			self.output[0] = 2

			if self.selected_preset == 3:
				# threshold val
				try:
					self.threshold = float(self.threshold_input1.GetValue())
				except ValueError:
					self.error()
					return
				if self.threshold > 6:
					self.error()
					return
				else:
					# adding 200 will make sure there is no neg val, put in output buffer
					self.output[1] = self.threshold + 200

				# ratio val
				try:
					self.ratio = float(self.ratio_input1.GetValue())
				except ValueError:
					self.error()
					return
				if self.ratio < 1 or self.ratio > 254:
					self.error()
					return
				else:
					# put ratio in output buffer, no mapping needed
					self.output[2] = self.ratio


			elif self.selected_preset == 4:
				# threshold val
				try:
					self.threshold = float(self.threshold_input2.GetValue())
				except ValueError:
					self.error()
					return
				if self.threshold > 6:
					self.error()
					return
				else:
					# adding 200 will make sure there is no neg val, put in output buffer
					self.output[1] = self.threshold + 200

				# ratio val
				try:
					self.ratio = float(self.ratio_input2.GetValue())
				except ValueError:
					self.error()
					return
				if self.ratio < 1 or self.ratio > 254:
					self.error()
					return
				else:
					# put ratio in output buffer, no mapping needed
					self.output[2] = self.ratio


		elif self.selected_effect == 3:
			# put eq as selected effect in the output buffer
			self.output[0] = 3

			if self.selected_preset == 5:
				# band gain vals
				try:
					self.low = float(self.low_input1.GetValue())
				except ValueError:
					self.error()
					return
				if self.low > 10 or self.low < -10:
					self.error()
					return
				else:
					self.output[1] = self.low + 10

				try:
					self.mid = float(self.mid_input1.GetValue())
				except ValueError:
					self.error()
					return
				if self.mid > 10 or self.mid < -10:
					self.error()
					return
				else:
					self.output[2] = self.mid + 10

				try:
					self.high = float(self.high_input1.GetValue())
				except ValueError:
					self.error()
					return
				if self.high > 10 or self.high < -10:
					self.error()
					return
				else:
					self.output[3] = self.high + 10

			elif self.selected_preset == 6:
				# band gain vals
				try:
					self.low = float(self.low_input2.GetValue())
				except ValueError:
					self.error()
					return
				if self.low > 10 or self.low < -10:
					self.error()
					return
				else:
					self.output[1] = self.low + 10

				try:
					self.mid = float(self.mid_input2.GetValue())
				except ValueError:
					self.error()
					return
				if self.mid > 10 or self.mid < -10:
					self.error()
					return
				else:
					self.output[2] = self.mid + 10

				try:
					self.high = float(self.high_input2.GetValue())
				except ValueError:
					self.error()
					return
				if self.high > 10 or self.high < -10:
					self.error()
					return
				else:
					self.output[3] = self.high + 10

			elif self.selected_preset == 7:
				# band gain vals
				try:
					self.low = float(self.low_input3.GetValue())
				except ValueError:
					self.error()
					return
				if self.low > 10 or self.low < -10:
					self.error()
					return
				else:
					self.output[1] = self.low + 10

				try:
					self.mid = float(self.mid_input3.GetValue())
				except ValueError:
					self.error()
					return
				if self.mid > 10 or self.mid < -10:
					self.error()
					return
				else:
					self.output[2] = self.mid + 10

				try:
					self.high = float(self.high_input3.GetValue())
				except ValueError:
					self.error()
					return
				if self.high > 10 or self.high < -10:
					self.error()
					return
				else:
					self.output[3] = self.high + 10

			elif self.selected_preset == 8:
				# band gain vals
				try:
					self.low = float(self.low_input4.GetValue())
				except ValueError:
					self.error()
					return
				if self.low > 10 or self.low < -10:
					self.error()
					return
				else:
					self.output[1] = self.low + 10

				try:
					self.mid = float(self.mid_input4.GetValue())
				except ValueError:
					self.error()
					return
				if self.mid > 10 or self.mid < -10:
					self.error()
					return
				else:
					self.output[2] = self.mid + 10

				try:
					self.high = float(self.high_input4.GetValue())
				except ValueError:
					self.error()
					return
				if self.high > 10 or self.high < -10:
					self.error()
					return
				else:
					self.output[3] = self.high + 10

			elif self.selected_preset == 9:
				# band gain vals
				try:
					self.low = float(self.low_input5.GetValue())
				except ValueError:
					self.error()
					return
				if self.low > 10 or self.low < -10:
					self.error()
					return
				else:
					self.output[1] = self.low + 10

				try:
					self.mid = float(self.mid_input5.GetValue())
				except ValueError:
					self.error()
					return
				if self.mid > 10 or self.mid < -10:
					self.error()
					return
				else:
					self.output[2] = self.mid + 10

				try:
					self.high = float(self.high_input5.GetValue())
				except ValueError:
					self.error()
					return
				if self.high > 10 or self.high < -10:
					self.error()
					return
				else:
					self.output[3] = self.high + 10

			elif self.selected_preset == 10:
				# band gain vals
				try:
					self.low = float(self.low_input6.GetValue())
				except ValueError:
					self.error()
					return
				if self.low > 10 or self.low < -10:
					self.error()
					return
				else:
					self.output[1] = self.low + 10

				try:
					self.mid = float(self.mid_input6.GetValue())
				except ValueError:
					self.error()
					return
				if self.mid > 10 or self.mid < -10:
					self.error()
					return
				else:
					self.output[2] = self.mid + 10

				try:
					self.high = float(self.high_input6.GetValue())
				except ValueError:
					self.error()
					return
				if self.high > 10 or self.high < -10:
					self.error()
					return
				else:
					self.output[3] = self.high + 10

			elif self.selected_preset == 11:
				# band gain vals
				try:
					self.low = float(self.low_input7.GetValue())
				except ValueError:
					self.error()
					return
				if self.low > 10 or self.low < -10:
					self.error()
					return
				else:
					self.output[1] = self.low + 10

				try:
					self.mid = float(self.mid_input7.GetValue())
				except ValueError:
					self.error()
					return
				if self.mid > 10 or self.mid < -10:
					self.error()
					return
				else:
					self.output[2] = self.mid + 10

				try:
					self.high = float(self.high_input7.GetValue())
				except ValueError:
					self.error()
					return
				if self.high > 10 or self.high < -10:
					self.error()
					return
				else:
					self.output[3] = self.high + 10

		if self.selected_preset == 0:
			self.error()
			return

		# count how many times we have submitted params so we can determine if we need to kill the running process before running again
		self.count = self.count + 1


		# OUTPUT > --------------------------------------------------------------------------------

		# console ------------------------------------------------------
	
		print self.output


		# display_effect.c ---------------------------------------------

		# if the c program to display the effects is already running, kill it, then run it again. (so we don't get values mixed up when we want to choose a different effect)
		if(self.count > 1):
			call(["sudo killall display_effect"], shell=True)

		# if it isn't already running...run it

		# send to 1wire c program to display on 7seg. build a string like used on the command line to pass the c file the args
		# & is to run in background so the gui is still responsive when wanting to change effects or parameters
		self.command = ["sudo ./display_effect {} {} {} {} &".format(str(self.output[0]), str(self.output[1]), str(self.output[2]), str(self.output[3]))]
		call(self.command, shell=True)


		# send_effect.c ------------------------------------------------

		print self.selected_effect 
		print self.selected_preset

		self.command2 = ["sudo ./send_effect {} {}".format(str(self.selected_effect), str(self.selected_preset))]
		call(self.command2, shell=True)
			




	def OnClose(self, event):
		""" This is called when the gui is closed. It clears the display if there are any characters on it,
		and then kills the program. Finally, it actually closes the gui window. """
		# if the c program is running when we close the gui, stop the c program
		if(self.count > 0):
			# we are closing the gui so if we had stuff displaying, lets clear the display
			call(["sudo ./display_effect -1"], shell=True)

			# now kill the process
			call(["sudo killall display_effect"], shell=True)

		self.Destroy()


# main()
if __name__ == "__main__":

	# instance of app
	app = wx.App()
    # instance of select_effect
    # (no parent, -1 to let wx choose an identifier, window title)
	frame = select_effect(None, -1, 'Digital Effect Suite')
	frame.Show()
    # while(1) to wait and handle events such as click of button, quitting, etc.
	app.MainLoop()