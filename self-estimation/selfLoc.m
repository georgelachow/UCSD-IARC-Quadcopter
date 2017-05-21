%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Self-localization script - quadcopter
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
clear
clf

% Load map
% grid = imread('grid.png');
% imshow(grid);
% hold on
figure(1);
axis([-10 10 -10 10]);
ax = gca;
ax.YTick = -10:1:10;
ax.XTick = -10:1:10;
grid on
hold on
line([-10 10],[0 0],'LineStyle','--');            % line for origin
line([0 0],[-10 10],'LineStyle','--');            % line for origin
line([-10 10],[10 10],'Color','red','LineWidth',3);            
line([-10 10],[-10 -10],'Color','green','LineWidth',3);          
title('IARC Arena');

% Initialize variables
trailSize = 5;
trail = zeros(trailSize,1);
turn = 1;
gyro = 0;
y = 0;
x = 0;

while(1)
% Get inputs
% vel = input('What is the velocity (m/s)? ');
% gyro = input('What is the angle (degs)? ');
% time = input('How long since last sampled (sec)? ');

vel = 0.5;
if mod(turn,3) == 0
    gyro = (359-0).*rand(1,1) + 0;
end
time = 1;

% Define anon functions
dist = @(v, t) v*t;
y_pos = @(d,theta) d*sind(theta);
x_pos = @(d,theta) d*cosd(theta);

distance = dist(vel, time);
new_y = y_pos(distance, gyro);
new_x = x_pos(distance, gyro);
x = new_x+x;
y = new_y+y;

% disp(sprintf('%f %f',x,y));

% Delete the tail of the trail
if turn > trailSize-1
    delete(trail(trailSize));l
end

% Plot and shift array
trail(1) = plot(x,y,'p');
trail = circshift(trail,1);

turn = turn + 1;
pause(.2);
end

% y roll
% x pitch
% rostopic echo /erlcopter/imu
% perception folder for source code