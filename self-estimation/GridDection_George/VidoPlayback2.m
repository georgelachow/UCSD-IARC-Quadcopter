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
    
    fig1 = figure(1)
    clf(fig1,'reset');
    set(fig1, 'Position', [0, 0, 1200, 1200]);
    imagesc(H);
    colorbar;
    
    P = houghpeaks(H,200,'threshold',ceil(0.5*max(H(:))));
    lines = houghlines(BW,theta,rho,P,'FillGap',30,'MinLength',50);
    
    
    fig2 = figure(2);
    clf(fig2,'reset');
    
    axis([0,640,0,480])
    
    set (gca,'Ydir','reverse');
    
    out = cat(3, BW,BW,BW);  
    imagesc(out);
    set(fig2, 'visible', 'off');
  
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
    out = getframe(fig2);
    out = out.cdata;
    hold off;

    close(fig2);
    
    %%%%% END
    
    % Display the annotated video frame using the video player object
    step(videoPlayer, out);

end