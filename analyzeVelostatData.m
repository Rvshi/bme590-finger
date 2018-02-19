%% analyze Velostat data
% initial testing: February 18, 2018

%% load data

data = load("test16.txt"); 
time = data(:,1); 
voltage = data(:,2); 

time = time/1000; 

%% plot data
figure(1); clf
plot(time, voltage)