#pragma once

// maximum number of ATEM inputs (using 21 because Skaarhoj's ATEM Arduino Library uses a 21 element array for atemTallyByIndexTallyFlags)
const int maxAtemInputs = 21;

const long timeBetweenAtemStateMessages = 1000;