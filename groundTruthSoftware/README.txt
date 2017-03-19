GROUND TRUTHING FOR LINE SEGMENTATION
----------------------------------------------------------

The purpose of this software is to create a more efficient and effective method for ground truthing historical newspapers that will prove useful for line segmentation.

WHY?

After using our current evaluation system (SLOTH), I found it difficult to apply it to the problem of segmenting articles. This is because the current ground truths used in these evaluations assume many of the later parts of this project have been completed, when they have not.

For instance, the ground truths assume a three-column article has already been linked together into a single article, but we have not reached this stage as of yet. Since it is critical that we evaulate every step of our progress, I wanted a better evaluation method for the segmentation of the newspaper into titles and articles. Furthermore, with the data collected through ground truthing, we can pass this data into a possible machine learning algorithm that can help improve the precision of our segmentation.

METHOD

This ground truthing software will involve a simple user interface by which the ground truther can easily use his/her keyboard to label a line as a title or an article.

The entire page is displayed to the user as this will help distinguish between titles and articles. Every iteration of the software will bound a particular line in black. This is a sign to the user that he/she is now looking at this line.

The user can then press different letters to label that line. Pressing the letter 'A' will label the line as an article. Pressing 'T' will label it as a title. Pressing 'S' wil label it as a sub-head.

These labels will be recorded in a data structure. In the meantime, as the lines are labelled, the software will automatically record the features of that line into another data structure.

At the end of the ground truthing process, the labels and the features will be stored in a file for later retrieval. This will also help facilitate any transitions to other pgoramming languages, such as Python. If we need to collect the data through Python, we can simply read from the file and store the information in a desired format.

THINGS TO THINK ABOUT

-- Histories - What if the user makes a mistake? We want them to correct it. An easy way would be to store the pointers to all the lines in an array. If the user wants to move back to undo a line, we can just move through the array. This will be done initially for the demo of the software.

-- Speed - We want this process to be fast. Therefore, at a later time, it might be good to draw each textBlock as a separate image and overlay these images together. Therefore, if the user is working on a particular line, we can just change the image that contains the corresponding textBlock. For now, we will draw the image each time.

-- Accessibility - The software assumes the user has OpenCV installed on their computer. If it comes to the point that some people cannot install OpenCV, we need to switch languages and find another way to do this.


THAT'S IT FOR NOW! THANKS FOR TRUTHING!
