clear
close all
clc

angleSweep = -45:135;
numLines = 1:2;
image = imread('grid6.png');
if size(image,3) > 1
    image = rgb2gray(image);
end

image = imrotate(image,42); %if you want to test rotation

thresh = 200;
for c = 1:size(image,2)
    tempCol= image(:,c);
    tempCol(tempCol < thresh) = 0;
    imageThresh(:,c) = tempCol;
end

imageCanny = edge(imageThresh,'canny');
[R, x] = radon(imageCanny', angleSweep);


filter = ones(6,6)/36;
Ravg = imfilter(R,filter);

Rang = max(Ravg,[],1);
Rloc = max(Ravg,[],2);

[angPks, angInd] = findpeaks(Rang);

[angMax1, angInd1] = max(angPks);
angPks1 = angPks(angPks ~= angMax1);

angMax2 = max(angPks1);
angInd2 = find(angPks == angMax2);

gridAngleIndicies = [find(Rang == Rang(angInd(angInd1))) find(Rang == Rang(angInd(angInd2)))];
gridAngles = angleSweep([gridAngleIndicies(1) gridAngleIndicies(2)]);

X = Ravg(:,gridAngleIndicies);

XS = X;
minDistances(numLines) = 0;
distances(numLines) = 10^-6;
iter = 1;
while(minDistances(1) < distances(1) || minDistances(2) < distances(2))
    for i = numLines
        locMeans(i) = mean(XS(:,i));
        locStds(i) = std(XS(:,i));
    end
    
    for i = numLines
        [locPks{i}, locInds{i}] = findpeaks(XS(:,i), 'MinPeakHeight', locMeans(i)+locStds(i));
    end
    
    for i = numLines
        peakDiff{i} = diff(locInds{i});
        peakMeans(i) = mean(peakDiff{i});
        peakStds(i) = std(peakDiff{i});
        if iter == 1
            distances(i) = peakMeans(i);
        end
        minDistances(i) = min(peakDiff{i});
    end
    
    for i = numLines
        XS(:,i) = smooth(XS(:,i));
    end
    iter = iter + 1;
end

locInds{1} = locInds{1}+angleSweep(1)-1;
locInds{2} = locInds{2}+angleSweep(1)-1;

truenorth = 90;

[angleDist, colAngleIndex] = min([norm(gridAngles(1)-truenorth), norm(gridAngles(2)-truenorth)])
rowAngleIndex = numLines(numLines ~= colAngleIndex)

outputMatrix = zeros(length(locInds{rowAngleIndex}),length(locInds{colAngleIndex}));



%% plot
figure;
imshow(image);
figure;
imshow(imageCanny)
figure;
imagesc(angleSweep, x, R)
figure;
plot(angleSweep,smooth(smooth(smooth(Rang))))
hold on
plot(angleSweep,[0,diff(Rang)])
figure;
plot(Rloc)
figure;
plot(XS(:,1))
figure;
plot(XS(:,2))