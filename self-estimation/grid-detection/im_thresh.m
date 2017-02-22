function [ img_thresh ] = im_thresh( img, whitePixelFraction )

doubleImg = double(img);

histogram = hist(doubleImg(:),256);

histSum = sum(histogram);
runSum = 0;
threshold = 0;

for i = size(histogram,2):-1:1
    runSum = runSum + histogram(i);
    if(runSum/histSum > whitePixelFraction)
        threshold = i;
        break;
    end
end

img_thresh = doubleImg;
for j = 1:1:size(img_thresh,1)
    for k = 1:1:size(img_thresh,2)
        if(img_thresh(j,k) >= threshold)
            img_thresh(j,k) = 255;
        else
            img_thresh(j,k) = 0;
        end
    end
end
 

end