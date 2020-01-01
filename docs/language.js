function changeLanguage(frameParent, lang) {
    frameParent.lang = lang;
    if (frameParent.lang === 'en') {
        frameParent.TeamLnk = 'team_en.html';
        frameParent.ZeGameLnk = 'zegame_en.html';
        frameParent.FeaturesLnk = 'features_en.html';
        frameParent.FeaturesToolsLnk = 'tools.html';
        frameParent.GamedocLnk = 'gamedoc\\gamedoc.htm';
        frameParent.GameScreenShotsLnk = 'screenshots.html';
        frameParent.DownloadLnk = 'download_en.html';
    } else {
        frameParent.TeamLnk = 'team.html';
        frameParent.ZeGameLnk = 'zegame.html';
        frameParent.FeaturesLnk = 'features_en.html';
        frameParent.FeaturesToolsLnk = 'tools.html';
        frameParent.GamedocLnk = 'gamedoc\\gamedoc.htm';
        frameParent.GameScreenShotsLnk = 'screenshots.html';
        frameParent.DownloadLnk = 'download_en.html';
    }
}

	