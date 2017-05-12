--- utils/gtf/gtfcalc.c.orig	2005-07-10 19:33:16 UTC
+++ utils/gtf/gtfcalc.c
@@ -64,7 +64,7 @@ static GTF_constants GC = {
 
 /*-------------------------- Implementation -------------------------------*/
 
-static double round(double v)
+static double gtf_round(double v)
 {
 	return floor(v + 0.5);
 }
@@ -84,9 +84,9 @@ static void GetInternalConstants(GTF_con
 ****************************************************************************/
 {
 	c->margin = GC.margin;
-	c->cellGran = round(GC.cellGran);
-	c->minPorch = round(GC.minPorch);
-	c->vSyncRqd = round(GC.vSyncRqd);
+	c->cellGran = gtf_round(GC.cellGran);
+	c->minPorch = gtf_round(GC.minPorch);
+	c->vSyncRqd = gtf_round(GC.vSyncRqd);
 	c->hSync = GC.hSync;
 	c->minVSyncBP = GC.minVSyncBP;
 	if (GC.k == 0)
@@ -140,13 +140,13 @@ void GTF_calcTimings(double hPixels,doub
 	vFreq = hFreq = dotClock = freq;
 
 	/* Round pixels to character cell granularity */
-	hPixels = round(hPixels / c.cellGran) * c.cellGran;
+	hPixels = gtf_round(hPixels / c.cellGran) * c.cellGran;
 
 	/* For interlaced mode halve the vertical parameters, and double
 	 * the required field refresh rate.
 	 */
 	if (wantInterlace) {
-		vLines = round(vLines / 2);
+		vLines = gtf_round(vLines / 2);
 		vFieldRate = vFreq * 2;
 		dotClock = dotClock * 2;
 		interlace = 0.5;
@@ -158,26 +158,26 @@ void GTF_calcTimings(double hPixels,doub
 
 	/* Determine the lines for margins */
 	if (wantMargins) {
-		topMarginLines = round(c.margin / 100 * vLines);
-		botMarginLines = round(c.margin / 100 * vLines);
+		topMarginLines = gtf_round(c.margin / 100 * vLines);
+		botMarginLines = gtf_round(c.margin / 100 * vLines);
 		}
 	else {
 		topMarginLines = 0;
 		botMarginLines = 0;
 		}
 
-	if (type != GTF_lockPF) {
+	if (type == GTF_lockVF || type == GTF_lockHF) {
 		if (type == GTF_lockVF) {
 			/* Estimate the horizontal period */
 			hPeriodEst = ((1/vFieldRate) - (c.minVSyncBP/1000000)) /
 				(vLines + (2*topMarginLines) + c.minPorch + interlace) * 1000000;
 
 			/* Find the number of lines in vSync + back porch */
-			vSyncBP = round(c.minVSyncBP / hPeriodEst);
+			vSyncBP = gtf_round(c.minVSyncBP / hPeriodEst);
 			}
-		else if (type == GTF_lockHF) {
+		else {
 			/* Find the number of lines in vSync + back porch */
-			vSyncBP = round((c.minVSyncBP * hFreq) / 1000);
+			vSyncBP = gtf_round((c.minVSyncBP * hFreq) / 1000);
 			}
 
 		/* Find the number of lines in the V back porch alone */
@@ -205,8 +205,8 @@ void GTF_calcTimings(double hPixels,doub
 
 	/* Find the number of pixels in the left and right margins */
 	if (wantMargins) {
-		leftMarginPixels = round(hPixels * c.margin) / (100 * c.cellGran);
-		rightMarginPixels = round(hPixels * c.margin) / (100 * c.cellGran);
+		leftMarginPixels = gtf_round(hPixels * c.margin) / (100 * c.cellGran);
+		rightMarginPixels = gtf_round(hPixels * c.margin) / (100 * c.cellGran);
 		}
 	else {
 		leftMarginPixels = 0;
@@ -224,7 +224,7 @@ void GTF_calcTimings(double hPixels,doub
 		/* Find the ideal blanking duty cycle */
 		idealDutyCycle = c.c - (c.m / hFreq);
 		}
-	else if (type == GTF_lockPF) {
+	else {
 		/* Find ideal horizontal period from blanking duty cycle formula */
 		idealHPeriod = (((c.c - 100) + (sqrt((pow(100-c.c,2)) +
 			(0.4 * c.m * (hTotalActivePixels + rightMarginPixels +
@@ -235,17 +235,17 @@ void GTF_calcTimings(double hPixels,doub
 		}
 
 	/* Find the number of pixels in blanking time */
-	hBlankPixels = round((hTotalActivePixels * idealDutyCycle) /
+	hBlankPixels = gtf_round((hTotalActivePixels * idealDutyCycle) /
 		((100 - idealDutyCycle) * 2 * c.cellGran)) * (2 * c.cellGran);
 
 	/* Find the total number of pixels */
 	hTotalPixels = hTotalActivePixels + hBlankPixels;
 
 	/* Find the horizontal back porch */
-	hBackPorch = round((hBlankPixels / 2) / c.cellGran) * c.cellGran;
+	hBackPorch = gtf_round((hBlankPixels / 2) / c.cellGran) * c.cellGran;
 
 	/* Find the horizontal sync width */
-	hSyncWidth = round(((c.hSync/100) * hTotalPixels) / c.cellGran) * c.cellGran;
+	hSyncWidth = gtf_round(((c.hSync/100) * hTotalPixels) / c.cellGran) * c.cellGran;
 
 	/* Find the horizontal sync + back porch */
 	hSyncBP = hBackPorch + hSyncWidth;
@@ -258,7 +258,7 @@ void GTF_calcTimings(double hPixels,doub
 		hPeriod = 1000 / hFreq;
 
 		/* Find the number of lines in vSync + back porch */
-		vSyncBP = round((c.minVSyncBP * hFreq) / 1000);
+		vSyncBP = gtf_round((c.minVSyncBP * hFreq) / 1000);
 
 		/* Find the number of lines in the V back porch alone */
 		vBackPorch = vSyncBP - c.vSyncRqd;
