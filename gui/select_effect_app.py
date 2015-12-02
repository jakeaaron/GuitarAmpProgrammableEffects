#!/usr/bin/python


# import gui library
try:
    import wx
except ImportError:
    raise ImportError,"The wxPython module is required to run this program."


class select_effect(wx.Frame):	# inherit from base class for gui windows
	
	# initialize class
	def __init__(self, parent, id, title):
		wx.Frame.__init__(self, parent, id, title=title, size=(300, 300))	# call wk.Frame constructor
		self.parent = parent	# useful to keep track of parent (usually a parent container)
		self.init_elements()


	# initialize/create all the gui widgets needed for the app
	def init_elements(self):

		self.panel = wx.Panel(self)     

		# set up sizers ---------------------------------------------------------------------
		# set sizer for the frame so we can resize window according to widgets/elements
		self.windowsizer = wx.BoxSizer()
		self.windowsizer.Add(self.panel, 1, wx.ALL | wx.EXPAND)        

		# set sizer for panel/grid layout for elements
		self.sizer = wx.GridBagSizer(5, 5)

		# set sizer for a nice border
		self.border = wx.BoxSizer()
		self.border.Add(self.sizer, 1, wx.ALL | wx.EXPAND, 5)


		self.select_effect_label = wx.StaticText(self.panel, label="Select your GAPE effect, bro!")
		self.sizer.Add(self.select_effect_label, pos=(1, 1))


		# layout the 3 effect selector buttons ----------------------------------------------
		delay_button = wx.Button(self.panel, label="Delay")
		# on click call click function and pass the sizer to it to add more windows for the appropriate effect
		self.Bind(wx.EVT_BUTTON, lambda event: self.on_delay_click(event, self.sizer, self.panel), delay_button)
		# add the button to the layout
		self.sizer.Add(delay_button, pos=(2, 1))

		comp_button = wx.Button(self.panel, label="Compressor")
		# on click call click function and pass the sizer to it to add more windows for the appropriate effect
		self.Bind(wx.EVT_BUTTON, lambda event: self.on_comp_click(event, self.sizer, self.panel), comp_button)
		# add button to the layout
		self.sizer.Add(comp_button, pos=(4, 1))

		eq_button = wx.Button(self.panel, label="Equalizer")
		# on click call click function and pass the sizer to it to add more windows for the appropriate effect
		self.Bind(wx.EVT_BUTTON, lambda event: self.on_eq_click(event, self.sizer, self.panel), eq_button)
		# add the button to the layout
		self.sizer.Add(eq_button, pos=(6, 1))


		# then tell app to use the sizers
		self.panel.SetSizerAndFit(self.border)
		self.SetSizerAndFit(self.windowsizer)



	# event handlers for selecting the effect -----------------------------------------------
	# this function is called if the delay effect is selected
	def on_delay_click(self, event, sizer, panel):
		# enter amount of delay --------------------------
		# enter time label
		self.time_input_label = wx.StaticText(panel, label="Enter amount of delay in seconds (max is 0.5)")
		sizer.Add(self.time_input_label, pos=(1, 5))
		# enter time field
		time_input = wx.TextCtrl(panel, value=u"0.5", size=(100, -1))
		sizer.Add(time_input, pos=(2, 5))

		# enter gain of the delayed signal ---------------
		# enter gain label
		self.time_input_label = wx.StaticText(panel, label="Enter the gain of the delayed signal (max is 1)")
		sizer.Add(self.time_input_label, pos=(3, 5))
		# enter gain field
		time_input = wx.TextCtrl(panel, value=u"1", size=(100, -1))
		sizer.Add(time_input, pos=(4, 5))

	def on_comp_click(self, event, sizer, panel):
		pass

	def on_eq_click(self, event, sizer, panel):
		pass




# main()
if __name__ == "__main__":

	# instance of app
	app = wx.App()
    # instance of class
    # (no parent, -1 to let wx choose an identifier, window title, size of gui window)
	frame = select_effect(None, -1, 'Select Your GAPE Effect')
	frame.Show()
    # while(1) to wait and handle events such as click of button, quitting, etc.
	app.MainLoop()