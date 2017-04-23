clear; 
close all;
% Read a video frame and run the detector.
videoFileReader = vision.VideoFileReader('field.avi');
videoFrame      = step(videoFileReader);

% Create a video player object for displaying video frames.
videoInfo    = info(videoFileReader);
videoPlayer  = vision.VideoPlayer('Position',[300 300 videoInfo.VideoSize+30]);

% Track the face over successive video frames until the video is finished.
while ~isDone(videoFileReader)

    % Extract the next video frame
    videoFrame = step(videoFileReader);

    %%%%% BEGIN
    
    % Convert to GrayScale
    I = rgb2gray(videoFrame);
    
    % Downsample by 50%
    resizedFrame = imresize(I, 0.5);
    
    % Median Filter
    %meanFrame = medfilt2(resizedFrame);
    
    % Filter out anything but white values
    whiteFilter = resizedFrame.*(resizedFrame > 0.75);
    
   
    
    % Canny Edge Detector
    sigma = sqrt(2);
    [BW, threshOut] = edge(whiteFilter, 'canny',[], sigma);
    
    % Hough Transform
    [H,theta,rho] = hough(BW, 'RhoResolution',1,'ThetaResolution',0.5);
    
    % Plot Hough Transform
    fig1= figure(1);
    clf(fig1,'reset');
    imshow(imadjust(mat2gray(H)),[],...
       'XData',theta,...
       'YData',rho,...
       'InitialMagnification','fit');
    xlabel('\theta (degrees)')
    ylabel('\rho')
    axis on
    axis normal
    hold on
    colormap(hot)

    % Find Peaks
    P = houghpeaks(H,200,'threshold',ceil(0.5*max(H(:))));
    
    % Plot Peaks
    x = theta(P(:,2));
    y = rho(P(:,1));
    plot(x,y,'s','linewidth',3,'MarkerSize',10,'color', 'red');
    %set(fig1, 'Position', [0, 0, 1200, 1200]);
    
    
    % Plot Quad Position, at x = 320, y = 240, p = 
    x = -90:90;
    y = 320*cosd(x)+240*sind(x)
    plot(x,y,'linewidth',3,'color', 'red');
    
    hold off;
    % Find Lines
    lines = houghlines(BW,theta,rho,P,'FillGap',30,'MinLength',50);
    
    %{
    % Plot Hough Lines
    fig2 = figure(2);
    clf(fig2,'reset');
    axis([0,640,0,480])
    set (gca,'Ydir','reverse');
    hold on;
    max_len = 0;
    for k = 1:length(lines)
       xy = [lines(k).point1; lines(k).point2];
       plot(xy(:,1),xy(:,2),'LineWidth',2,'Color','green');

       % Plot beginnings and ends of lines
       plot(xy(1,1),xy(1,2),'x','LineWidth',2,'Color','yellow');
       plot(xy(2,1),xy(2,2),'x','LineWidth',2,'Color','red');

       % Determine the endpoints of the longest line segment
       len = norm(lines(k).point1 - lines(k).point2);
       if ( len > max_len)
          max_len = len;
          xy_long = xy;
       end
    end
    hold off;
    %}
    
    %%%%% END
    
    % Display the annotated video frame using the video player object
    step(videoPlayer, whiteFilter);

end

% Release resources
release(videoFileReader);
release(videoPlayer);