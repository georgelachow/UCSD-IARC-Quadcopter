clear;

RGB = imread('grid.png');
%figure(1);
%imshow(RGB);
I = rgb2gray(RGB);
%figure(2);
%imshow(I);

BW = edge(I, 'canny');
%figure(4);
%imshow(BW);

[H,theta,rho] = hough(BW);
figure(5);
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

P = houghpeaks(H,100,'threshold',ceil(0.3*max(H(:))));
x = theta(P(:,2));
y = rho(P(:,1));
plot(x,y,'s','color','black');

lines = houghlines(BW,theta,rho,P,'FillGap',5,'MinLength',7);
figure(6)
%imshow(RGB) 
hold on

max_len = 0;
point1 = lines(1).point1;
point2 = lines(1).point2; 
for k = 2:length(lines)
    % Vertical Lines
    temp1 = lines(k).point1;
    temp2 = lines(k).point2;
    while temp1(:,2) == temp2(:,2) && k < length(lines)-1
        k = k + 1;
        point1 = temp1
        point2 = lines(k).point2; 
    end
    xy = [point1;point2];
    plot(xy(:,1),xy(:,2),'LineWidth',2,'Color','green');
    if k == length(lines)-1
        break;
    end
end
            
            
    




%{
for k = 1:length(lines)
   point1 = lines(k).point1;
   point2 = lines(k).point2;   
   
   xy = [point1;point2];
   %xy = [lines(k).point1; lines(k).point2];
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
%}
% highlight the longest line segment
%plot(xy_long(:,1),xy_long(:,2),'LineWidth',2,'Color','red');