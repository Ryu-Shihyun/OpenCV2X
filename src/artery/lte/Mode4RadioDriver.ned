package artery.lte;

import artery.nic.IRadioDriver;

simple Mode4RadioDriver like IRadioDriver
{
	parameters:
		@class(Mode4RadioDriver);
		@signal[ChannelLoad](type=double);
		@signal[ChannelOccupancy](type=double);
		double channelLoadReportInterval = default(0.1s) @unit(s);
		bool addStartUpDelay = default(false);
		int packetLatencyLimit = default(1000);

		@signal[camGen];
        @statistic[camGen](title="Message Arrival time"; source="camGen"; record=mean,vector);

	gates:
		inout upperLayer;
		input lowerLayerIn;
		output lowerLayerOut;
		output properties;
}
