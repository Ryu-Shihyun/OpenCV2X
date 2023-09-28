package artery.nic;

moduleinterface IRadioDriver
{
	parameters:
		@signal[ChannelLoad](type=double);
		@signal[ChannelOccupancy](type=double);

	gates:
		inout upperLayer;
		output properties;
}
