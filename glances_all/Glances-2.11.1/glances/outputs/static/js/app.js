var glancesApp = angular.module('glancesApp', ['glances.config', 'cfp.hotkeys'])

.value('CONFIG', {})
.value('ARGUMENTS', {})

.config(function (hotkeysProvider) {
    hotkeysProvider.useNgRoute = false;
    hotkeysProvider.includeCheatSheet = false;
})

.run(function ($rootScope, GlancesStats) {
    $rootScope.title = "Glances";

    $rootScope.$on('data_refreshed', function (event, data) {
        $rootScope.title = data.stats.system.hostname + ' - Glances';
    });

    GlancesStats.init();
});
