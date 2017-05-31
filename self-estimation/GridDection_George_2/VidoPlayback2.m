clear; 
close all;
% Read a video frame and run the detector.
videoFileReader = vision.VideoFileReader('SampleVideos/Taking_Off_at_Topright.mov');
videoFrame      = step(videoFileReader);

% Create a video player object for displaying video frames.
videoInfo    = info(videoFileReader);
videoPlayer  = vision.VideoPlayer('Position',[400 400 videoInfo.VideoSize+30]);

% Track the face over successive video frames until the video is finished.
while ~isDone(videoFileReader)

    % Extract the next video frame
    videoFrame = step(videoFileReader);
    videoFrame = step(videoFileReader);
    videoFrame = step(videoFileReader);
    videoFrame = step(videoFileReader);

    %%%%% BEGIN
    
    % Convert to GrayScale
    I = rgb2gray(videoFrame);
    
    % Downsample by 50%
    resizedFrame = imresize(I, 0.5);
    
    % Median Filter
    %meanFrame = medfilt2(resizedFrame);
    
    % Filter out anything but white values
    cannyInput = resizedFrame.*(resizedFrame > 0.9);
    
    % Highpass Filter
    %cannyInput = imfilter(resizedFrame, fspecial('average', 20), 'replicate'); 

    
    % Canny Edge Detector
    sigma = sqrt(2);
    %[BW, threshOut] = edge(cannyInput, 'canny',[], sigma);
    [BW, threshOut] = edge(cannyInput, 'Roberts');
    
    % Hough Transform
    [H,theta,rho] = hough(BW, 'RhoResolution',1,'ThetaResolution',0.5);
    
    % Hough Space Video
    fig1= figure(1);
    clf(fig1,'reset');
    set(fig1, 'Position', [0, 0, 1120, 840]);
    set(fig1, 'visible', 'off');
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
    
    
    P = houghpeaks(H,200,'threshold',ceil(0.5*max(H(:))));
    
    % Plot Peaks
    x = theta(P(:,2));
    y = rho(P(:,1));
    plot(x,y,'s','linewidth',3,'MarkerSize',10,'color', 'red');
    % Plot Quad Position, at x = 320, y = 240, p = 
    [yMax,xMax] = size(resizedFrame);
    x = -90:90;
    y = (xMax/2)*cosd(x)+(yMax/2)*sind(x);
    plot(x,y,'linewidth',2,'color', 'red');
    hold off;
    
    
    lines = houghlines(BW,theta,rho,P,'FillGap',30,'MinLength',50);
    
    
    % Original Video
    fig2 = figure(2);
    clf(fig2,'reset');
    axis([0,640,0,480])
    set (gca,'Ydir','reverse');
    out = cat(3, resizedFrame,resizedFrame,resizedFrame);  
    %out = cat(3, BW,BW,BW);  
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
    
    hold off;
    
    fig3 = figure(3);
    clf(fig3,'reset');
    image(videoFrame);
    set(fig3, 'visible', 'off');
    
    fig4 = figure(4);
    clf(fig4,'reset');
    out = cat(3, BW,BW,BW);  
    imagesc(out);
    set(fig4, 'visible', 'off');
    
    out1 = getframe(fig1);
    out2 = getframe(fig2);
    out3 = getframe(fig3);
    out4 = getframe(fig4);
    row1 = vertcat(out3.cdata, out4.cdata);
    %row2 = horzcat(out2.cdata, out1.cdata);
    houghPlot = imresize(out1.cdata, 1);
    out = horzcat(row1, houghPlot);
    
    close(fig1);
    close(fig2);
    close(fig3);
    close(fig4);
    
    %out = out.cdata;
    
    %%%%% END
    
    
    % Display the annotated video frame using the video player object
    step(videoPlayer, out);

end