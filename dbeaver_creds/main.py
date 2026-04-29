"""CLI entry point: ``dbeaver-creds [PATH]`` prints the decrypted JSON to stdout."""
from __future__ import annotations

from pathlib import Path

from dbeaver_creds._dbeaver_creds import get_dbeaver_credentials
import click


@click.command(context_settings={'help_option_names': ['-h', '--help']})
@click.argument(
    'path',
    required=False,
    type=click.Path(dir_okay=False, exists=True, path_type=Path, readable=True),
)
def main(path: Path | None) -> None:
    """Decrypt and display DBeaver's credentials-config.json.

    If PATH is given, it is read from there. Otherwise the platform-default
    location is used.
    """  # noqa: DOC501
    try:
        payload = get_dbeaver_credentials(path)
    except RuntimeError as exc:
        raise click.ClickException(str(exc)) from exc
    click.echo(payload)
